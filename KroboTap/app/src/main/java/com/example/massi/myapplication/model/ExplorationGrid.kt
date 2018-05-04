package com.example.massi.myapplication.model


public enum class ExplorationCellStatus {
    UNEXPLORED,
    BLOCK,
    GOAL,
    DOCKING_STATION,
    ROBOT,
    UNTRAVERSABLE
}

/**
 * Initialize the exploration grid
 *
 * Every cell alcks completely by any element
 */
public class ExplorationGrid(rows: Int, columns: Int) : AbstractItemGrid<ExplorationCellStatus>(rows, columns, {p -> getDefaultSet()}) {

    companion object {

        inline fun getDefaultSet() : MutableSet<ExplorationCellStatus> {
            val result  = HashSet<ExplorationCellStatus>()
            return result
        }
    }

    init {

    }

    /**
     * The position of the robot, if repsent in the grid.
     *
     * Null otherwise
     */
    val robot: Point?
        get() {
            for (row in 0..(rows-1)) {
                for (col in 0..(columns-1)) {
                    if (ExplorationCellStatus.ROBOT in this[row, col]) {
                        return Point(row, col)
                    }
                }
            }
            return null
        }
}