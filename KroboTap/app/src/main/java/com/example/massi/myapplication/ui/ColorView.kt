package com.example.massi.myapplication.ui

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.view.View
import android.widget.ImageView
import com.example.massi.myapplication.R
import com.example.massi.myapplication.WhenClauseException
import com.example.massi.myapplication.model.ExplorationCellStatus
import com.example.massi.myapplication.model.Point
import com.example.massi.myapplication.model.WorldCellStatus
import com.example.massi.myapplication.model.WorldGrid
import java.util.logging.Logger

public class CellImageView(context: Context, row: Int, column: Int, startImage: Int): ImageView(context) {

    companion object {
        val LOG = Logger.getLogger(CellImageView::class.java.name)
    }

    public val row = row
    public val column = column

    public val point
        get() = Point(y=row, x=column)

    init {
        this.changeImage(startImage)
    }

    /**
     * update the view redendering depending on the content of buildingMap in the point p
     */
    public fun redraw(buildingMap: WorldGrid, p: Point) {
        val content = buildingMap[p]

        if (content.isEmpty()) {
            // Set the ImageView image programmatically
            this.changeImage(R.drawable.world_traversable)
        } else if (!buildingMap.isTraversable(p)) {
            this.changeImage(R.drawable.world_untraversable)
        } else if (WorldCellStatus.ROBOT in content && WorldCellStatus.GOAL in content) {
            this.changeImage(R.drawable.world_robot_goal)
        } else if (WorldCellStatus.ROBOT in content) {
            this.changeImage(R.drawable.world_robot)
        } else if (WorldCellStatus.GOAL in content) {
            this.changeImage(R.drawable.world_goal)
        } else {
            LOG.info(String.format("set is %s", content))
            throw WhenClauseException("invalid scenario")
        }
    }

    private fun changeImage(newResourceImage: Int) {
        this.setImageResource(newResourceImage);
    }
}
