package com.example.massi.myapplication.ui

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.view.View
import android.widget.ImageView
import com.example.massi.myapplication.WhenClauseException
import com.example.massi.myapplication.model.ExplorationCellStatus
import com.example.massi.myapplication.model.Point
import com.example.massi.myapplication.model.WorldCellStatus
import com.example.massi.myapplication.model.WorldGrid
import java.util.logging.Logger

/**
 * Represents a view filled with a solid color
 */
class ColorView(context: Context, row: Int, column: Int, acolor: Int): View(context) {

    companion object {
        val LOG = Logger.getLogger(SetupGridFragment::class.java.name)
    }

    init {
        this.setBackgroundColor(acolor)
    }

    public val row = row
    public val column = column

    public val point
        get() = Point(y=row, x=column)

    public var color : Int = acolor
        set(value) {
            field = value
            this.setBackgroundColor(color)
        }

    /**
     * update the view redendering depending on the content of buildingMap in the point p
     */
    public fun redraw(buildingMap: WorldGrid, p: Point) {
        val content = buildingMap[p]

        if (content.isEmpty()) {
            color = Color.WHITE
        } else if (!buildingMap.isTraversable(p)) {
            color = Color.RED
        } else if (WorldCellStatus.ROBOT in content && WorldCellStatus.GOAL in content) {
            color = Color.YELLOW
        } else if (WorldCellStatus.ROBOT in content) {
            color = Color.GREEN
        } else if (WorldCellStatus.GOAL in content) {
            color = Color.BLUE
        } else {
            LOG.info(String.format("set is %s", content))
            throw WhenClauseException("invalid scenario")
        }

    }


}