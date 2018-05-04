package com.example.massi.myapplication.model

public enum class WorldCellStatus {
    ROBOT,
    DOCKING_STATION,
    CRATE,
    GOAL,
    UNTRAVERSABLE
}

/**
 * Represents a grid used when building the actual world
 *
 * @param rows the number of rows the grid has
 * @param columns the number of columns the grid has
 */
class WorldGrid(rows: Int, columns: Int) : AbstractItemGrid<WorldCellStatus>(rows, columns, {p -> getDefaultSet()}) {

    companion object {

        inline fun getDefaultSet() : MutableSet<WorldCellStatus> {
            val result  = HashSet<WorldCellStatus>()
            return result
        }

    }

    var robot: Point? = null

    /**
     * Adds the content but if the robot is added, we keep track in #robot variable
     */
    override fun addContent(p: Point, t: WorldCellStatus) {
        super.addContent(p, t)

        if (t == WorldCellStatus.ROBOT) {
            //we create a new object, since p may change afterwards
            this.robot = Point(p)
        }
    }

    /**
     * Remove the content but if the robot is removed, we keep track in #robot variable
     */
    override fun removeContent(p: Point, t: WorldCellStatus) {
        super.removeContent(p, t)

        if (t == WorldCellStatus.ROBOT) {
            this.robot = null
        }
    }

    /**
     * Clear all the content of the cell
     */
    override fun clearContent(p: Point) {
        super.clearContent(p)

        if (this.robot == p) {
            this.robot = null
        }
    }

    /**
     * @return true if the cell can be traversed by the robot, false toehrwise
     */
    fun isTraversable(p: Point) : Boolean {
        return WorldCellStatus.UNTRAVERSABLE !in this[p]
    }

    /**
     * Move the robot from the previous position to another one
     *
     * The function takes care also of the scenario where the robot is nowhere to be found since this type
     * of grid may have zero robot positions
     */
    fun moveRobot(newDestination: Point) {
        val robotPos = this.robot?.let {
            this.removeContent(it, WorldCellStatus.ROBOT)
            this.addContent(newDestination, WorldCellStatus.ROBOT)
        } ?: this.addContent(newDestination, WorldCellStatus.ROBOT)
    }



}