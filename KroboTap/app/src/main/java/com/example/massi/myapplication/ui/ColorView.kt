package com.example.massi.myapplication.ui

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.view.View
import com.example.massi.myapplication.WhenClauseException
import com.example.massi.myapplication.model.ExplorationCellStatus
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

    public var color : Int = acolor
        set(value) {
            field = value
            this.setBackgroundColor(color)
        }

    public fun redraw(content: Set<ExplorationCellStatus>) {
        if (content.isEmpty()) {
            color = Color.WHITE
        } else if (ExplorationCellStatus.UNTRAVERSABLE in content) {
            color = Color.RED
        } else if (ExplorationCellStatus.ROBOT in content && ExplorationCellStatus.GOAL in content) {
            color = Color.YELLOW
        } else if (ExplorationCellStatus.ROBOT in content) {
            color = Color.GREEN
        } else if (ExplorationCellStatus.GOAL in content) {
            color = Color.BLUE
        } else {
            LOG.info(String.format("set is %s", content))
            throw WhenClauseException("invalid scenario")
        }

    }


}