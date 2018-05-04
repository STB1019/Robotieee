package com.example.massi.myapplication.ui

import android.app.ActionBar
import android.graphics.Color
import android.os.Bundle
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageView
import android.widget.TableLayout
import android.widget.TableRow
import com.example.massi.myapplication.*
import com.example.massi.myapplication.model.ExplorationCellStatus
import com.example.massi.myapplication.model.ExplorationGrid
import com.example.massi.myapplication.model.Point
import com.example.massi.myapplication.model.WorldCellStatus
import kotlinx.android.synthetic.main.setup_grid_fragment_layout.*
import kotlinx.android.synthetic.main.setup_grid_fragment_layout.view.*
import kotlinx.android.synthetic.main.setup_grid_options_fragment_layout.*
import kotlinx.android.synthetic.main.start_fragment_layout.*
import kotlinx.android.synthetic.main.start_fragment_layout.view.*
import java.util.logging.Logger

/**
 * Represents the fragment specifying the view used to create the world of the robot
 *
 * In particular in this view the user can set robot, goals and untraversables positions
 */
class SetupGridFragment : Fragment() {

    private enum class MapStatus {
        START,
        ROBOT_SET,
        GOAL_SET,
        BLOCKED_SET
    }

    companion object {
        val LOG = Logger.getLogger(SetupGridFragment::class.java.name)
    }

    private var chooseButtonPressed: String = ""
    private var table: Array<Array<Button>> = arrayOf()
    private var state: MapStatus = MapStatus.START
    private var isRobotPresent = false

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.setup_grid_fragment_layout, container, false)

        this.createTable(view, this.krobot.builingMap.rows, this.krobot.builingMap.columns)

        view.chooseRobot.setOnClickListener({ e -> this.krobot.writeStream.onNext(KrobotAppButtonPressed(view.chooseRobot)) })
        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.chooseRobot })
                .subscribe({e ->
                    LOG.info("robot is pressed")
                    this.chooseRobot.isPressed = !this.chooseRobot.isPressed
                    this.state = if (this.state == MapStatus.ROBOT_SET) MapStatus.START else MapStatus.ROBOT_SET
                })

        view.chooseGoal.setOnClickListener({ e -> this.krobot.writeStream.onNext(KrobotAppButtonPressed(view.chooseGoal)) })
        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.chooseGoal })
                .subscribe({e -> this.state = if (this.state == MapStatus.GOAL_SET) MapStatus.START else MapStatus.GOAL_SET })

        view.chooseUntraversable.setOnClickListener({ e -> this.krobot.writeStream.onNext(KrobotAppButtonPressed(view.chooseUntraversable)) })
        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.chooseUntraversable })
                .subscribe({e -> this.state = if (this.state == MapStatus.BLOCKED_SET) MapStatus.START else MapStatus.BLOCKED_SET })

        return view
    }

    private fun createTable(view: View, rows: Int, columns: Int) {
        //the table layout needs to be put within a framelayout, hence we need to use LayoutParams of TableLayout
        //val params : TableLayout.LayoutParams = TableLayout.LayoutParams(TableLayout.LayoutParams.MATCH_PARENT, TableLayout.LayoutParams.MATCH_PARENT)
        //params.setMargins(5,5,5,5)

        //LOG.info("rows are %d while columns are %d", rows, columns)

        view.tableLayout.isShrinkAllColumns = true
        //view.tableLayout.isStretchAllColumns = true

        var nextId = 10
        for (row in 0..(rows-1)) {
            LOG.info("Building row %d in table", row)
            val tableRow = TableRow(this.activity)
            tableRow.id = nextId++
            tableRow.layoutParams = TableLayout.LayoutParams(TableLayout.LayoutParams.MATCH_PARENT, TableLayout.LayoutParams.MATCH_PARENT)

            for (col in 0..(columns-1)) {
                LOG.info("Building cell y=%d x=%d", row, col)
                val cell = ColorView(this.context, row, col, Color.BLACK)
                cell.id = nextId++
                cell.layoutParams = TableRow.LayoutParams(TableRow.LayoutParams.WRAP_CONTENT, TableRow.LayoutParams.WRAP_CONTENT)

                this.krobot.readStream
                        .filter({ e -> e is KrobotAppButtonPressed && e.source == cell })
                        .subscribe({e ->
                            LOG.info("cell is %s and state is %s", cell.point, this.state)
                            //represents all the cell which we need to redraw
                            val cellsToChange = ArrayList<Point>()
                            when (this.state) {
                                MapStatus.START -> {
                                }
                                MapStatus.ROBOT_SET -> {
                                    //we set the robot only fi the cell selected is not untraversable
                                    if (this.krobot.builingMap.isTraversable(cell.point)) {
                                        //move the robot to the pointed cell (and removes it from the previous one)
                                        this.krobot.builingMap.robot?.let {
                                            cellsToChange.add(it)
                                        }
                                        cellsToChange.add(cell.point)
                                        this.krobot.builingMap.moveRobot(cell.point)
                                    }
                                }
                                MapStatus.GOAL_SET -> {
                                    //we set the goal only fi the cell selected is not untraversable
                                    if (this.krobot.builingMap.isTraversable(cell.point)) {
                                        if (WorldCellStatus.GOAL in this.krobot.builingMap[cell.row, cell.column]) {
                                            this.krobot.builingMap.removeContent(cell.point, WorldCellStatus.GOAL)
                                        } else {
                                            this.krobot.builingMap.addContent(cell.point, WorldCellStatus.GOAL)
                                        }
                                        cellsToChange.add(cell.point)
                                    }
                                }
                                MapStatus.BLOCKED_SET -> {
                                    //set the untraversable
                                    if (this.krobot.builingMap.isTraversable(cell.point)) {
                                        this.krobot.builingMap.removeContent(cell.point, WorldCellStatus.UNTRAVERSABLE)
                                    } else {
                                        //if we need to add untraverasble, we remove everything else from the cell
                                        this.krobot.builingMap.clearContent(cell.point)
                                        this.krobot.builingMap.addContent(cell.point, WorldCellStatus.UNTRAVERSABLE)
                                    }
                                    cellsToChange.add(cell.point)
                                }
                            }

                            //we need to notify the cells which needs to be redrawn
                            LOG.info("cells to change are %s", cellsToChange)
                            for (cellToChange in cellsToChange) {
                                this.krobot.writeStream.onNext(KrobotAppPointEvent(cell, cellToChange))
                            }

                        })

                this.krobot.readStream
                        .filter({ e -> e is KrobotAppPointEvent && e.point == cell.point})
                        .map({e -> e as KrobotAppPointEvent})
                        .subscribe({ e ->
                            cell.redraw(this.krobot.builingMap, cell.point)
                        })

                cell.setOnClickListener({ e -> this.krobot.writeStream.onNext(KrobotAppButtonPressed(cell)) })


                LOG.info(String.format("the id is %d", cell.id))
                //cell.layoutParams = params
                tableRow.addView(cell)
            }


            view.tableLayout.addView(tableRow)
        }

    }

}

