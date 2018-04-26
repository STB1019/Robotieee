package com.example.massi.myapplication.model

public enum class SokobanCellStatus {
    PLAYER,
    BLOCK,
    GOAL,
    DOCKING_STATION,
    UNTRAVERSABLE
}

public class SokobanGrid(rows: Int, columns: Int) : AbstractItemGrid<SokobanCellStatus>(rows, columns, {p -> getDefaultSet()}) {

    companion object {

        inline fun getDefaultSet() : MutableSet<SokobanCellStatus> {
            return HashSet<SokobanCellStatus>()
        }
    }

    init {

    }
}