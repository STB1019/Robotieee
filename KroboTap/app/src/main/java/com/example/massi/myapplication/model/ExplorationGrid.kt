package com.example.massi.myapplication.model


public enum class ExplorationCellStatus {
    UNEXPLORED,
    BLOCK,
    GOAL,
    DOCKING_STATION,
    UNTRAVERSABLE
}

public class ExplorationGrid(rows: Int, columns: Int) : AbstractItemGrid<ExplorationCellStatus>(rows, columns, {p -> getDefaultSet()}) {

    companion object {

        inline fun getDefaultSet() : MutableSet<ExplorationCellStatus> {
            val result  = HashSet<ExplorationCellStatus>()
            result.add(ExplorationCellStatus.UNEXPLORED)
            return result
        }
    }

    init {

    }
}