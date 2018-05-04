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
import kotlinx.android.synthetic.main.setup_grid_fragment_layout.*
import kotlinx.android.synthetic.main.setup_grid_fragment_layout.view.*
import kotlinx.android.synthetic.main.setup_grid_options_fragment_layout.*
import kotlinx.android.synthetic.main.start_fragment_layout.*
import kotlinx.android.synthetic.main.start_fragment_layout.view.*
import java.util.logging.Logger

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

        this.createTable(view, this.krobot.explorationMap.rows, this.krobot.explorationMap.columns)

        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.chooseRobot })
                .subscribe({e -> this.state = if (this.state == MapStatus.ROBOT_SET) MapStatus.START else MapStatus.ROBOT_SET })
        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.chooseGoal })
                .subscribe({e -> this.state = if (this.state == MapStatus.GOAL_SET) MapStatus.START else MapStatus.GOAL_SET })
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
            LOG.info(String.format("Building row %d in table", row))
            val tableRow = TableRow(this.activity)
            tableRow.id = nextId++
            tableRow.layoutParams = TableLayout.LayoutParams(TableLayout.LayoutParams.MATCH_PARENT, TableLayout.LayoutParams.MATCH_PARENT)

            for (col in 0..(columns-1)) {
                LOG.info(String.format("Building cell y=%d x=%d", row, col))
                val cell = ColorView(this.context, row, col, Color.BLACK)
                cell.id = nextId++
                cell.layoutParams = TableRow.LayoutParams(TableRow.LayoutParams.WRAP_CONTENT, TableRow.LayoutParams.WRAP_CONTENT)

                this.krobot.readStream
                        .filter({ e -> e is KrobotAppButtonPressed && e.source == cell })
                        .subscribe({e ->
                            when (this.state) {
                                MapStatus.START -> {
                                }
                                MapStatus.ROBOT_SET -> {
                                    //we set the robot only fi the cell selected is not untraversable
                                    if (ExplorationCellStatus.UNTRAVERSABLE !in this.krobot.explorationMap[cell.row, cell.column]) {
                                        if (isRobotPresent) {
                                            //the robot is inside the map, hence we need to move it to the cell pointeed by the user
                                            val robotPosition = this.krobot.explorationMap.robot
                                            this.krobot.explorationMap[robotPosition!!.x, robotPosition.y].remove(ExplorationCellStatus.ROBOT)
                                            this.krobot.explorationMap[row, col].add(ExplorationCellStatus.ROBOT)
                                        } else {
                                            //the robot is not inside the map, hence we need to manually put it!
                                            this.krobot.explorationMap[row, col].add(ExplorationCellStatus.ROBOT)
                                            isRobotPresent = true
                                        }
                                    }

                                }
                                MapStatus.GOAL_SET -> {
                                    //we set the goal only fi the cell selected is not untraversable
                                    if (ExplorationCellStatus.UNTRAVERSABLE !in this.krobot.explorationMap[cell.row, cell.column]) {
                                        if (ExplorationCellStatus.GOAL in this.krobot.explorationMap[cell.row, cell.column]) {
                                            this.krobot.explorationMap[cell.row, cell.column].remove(ExplorationCellStatus.GOAL)
                                        } else {
                                            this.krobot.explorationMap[cell.row, cell.column].add(ExplorationCellStatus.GOAL)
                                        }
                                    }
                                }
                                MapStatus.BLOCKED_SET -> {
                                    //set the untraversable
                                    if (ExplorationCellStatus.UNTRAVERSABLE in this.krobot.explorationMap[cell.row, cell.column]) {
                                        this.krobot.explorationMap[cell.row, cell.column].remove(ExplorationCellStatus.UNTRAVERSABLE)
                                    } else {
                                        //if we need to add untraverasble, we remove everything else from the cell
                                        if (ExplorationCellStatus.ROBOT in this.krobot.explorationMap[cell.row, cell.column]) {
                                            this.isRobotPresent = false
                                        }
                                        this.krobot.explorationMap[cell.row, cell.column].clear()
                                        this.krobot.explorationMap[cell.row, cell.column].add(ExplorationCellStatus.UNTRAVERSABLE)
                                    }

                                }
                            }

                            cell.redraw( this.krobot.explorationMap[cell.row, cell.column])
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

