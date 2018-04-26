package com.example.massi.myapplication.model

import com.example.massi.myapplication.WhenClauseException
import java.lang.StringBuilder

public enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
}

/**
 * Represents a 2D matrix
 *
 * (0,0) represents the top left corner while (rows, cols) represents the bottom right cell
 *
 * @param[rows] the number of rows of the matrix. Rows are also known as "y"
 * @param[columns] the number fo column of the matrix. Columns are also known as "x"
 * @param[cellPopulator] lambda allowing you to define the content of each cell
 */
public open abstract class AbstractGrid<T>(rows: Int, columns: Int, cellPopulator: (Point) -> T){

    companion object {

    }

    val MINPOINT = Point(0,0)
    val MAXPOINT = Point(rows-1, columns-1)

    val rows = rows
        public get
    val columns = columns
        public get
    val matrix: MutableList<T> = MutableList(rows*columns, {i -> cellPopulator(convertIndexInto2DCoordinate(i))})

    init {
    }

    /**
     * Convert a coordinate of type (y,x) into one compliant with a list (only one index
     *
     * @param[y] the row coordinate
     * @param[x] the column coordinate
     * @reutrn the index in the list of the point (y,x)
     */
    private inline fun convert2DCoordinateIntoIndex(y: Int, x: Int): Int {
        return y*rows + x
    }

    private inline fun convertIndexInto2DCoordinate(i: Int): Point {
        return Point(i % columns, i / columns)
    }

    /**
     * Overloading of []. Retrieve the vlaue within the cell
     *
     * @param[y] the row of the cell to fetch
     * @param[x] the column of the cell to fetch
     * @return the content fo the cell (y,x)
     */
    operator fun get(y: Int, x: Int): T {
        return this.matrix[convert2DCoordinateIntoIndex(y, x)]
    }

    /**
     * Like get, but accepts a point
     *
     * @param[p] the point representing the coordinates of the cell
     * @return the cell content requested
     */
    operator fun get(p: Point) : T {
        return this[p.y, p.x]
    }

    /**
     * Overloading of []. Set the cell
     *
     * @param[y] the row of the cell to set
     * @param[x] the column of the cell to set
     * @param[value] the value the cell will have after this operation
     */
    operator fun set(y: Int, x:Int, value: T) {
        this.matrix[convert2DCoordinateIntoIndex(y, x)] = value
    }

    /**
     * Like set, but accepts a point
     *
     * @param[p] the point representing the coordiantes of the cell we wish to change
     * @param[value] the new content of the cell
     */
    operator fun set(p: Point, value: T) {
        this[p.y, p.x] = value
    }

    /**
     * Obtain the cell in the given direction relative to the given cell
     *
     * @param[base] the cell we consider
     * @param[direction] the direction we desire to move
     * @return a Point to the desired cell
     */
    fun cellOf(base: Point, direction: Direction) : Point{
        when (direction) {
            Direction.DOWN  -> { return (base + Point(+1, 0)).clamp(MINPOINT, MAXPOINT) }
            Direction.UP -> {return ( base + Point(-1, 0)).clamp(MINPOINT, MAXPOINT)}
            Direction.LEFT -> {return (base + Point(0, -1)).clamp(MINPOINT, MAXPOINT)}
            Direction.RIGHT -> {return (base + Point(0, +1)).clamp(MINPOINT, MAXPOINT)}
            else -> throw WhenClauseException(direction.toString())
        }
    }

    /**
     * Create a string representation of the grid
     */
    override fun toString(): String {
        val sb = StringBuilder()
        for (y in 1..rows) {
            for (x in 1..columns) {
                sb.append("<y=%02d, x=%02d, cell=%s>", y, x, this[y,x])
            }
        }
        return sb.toString()
    }
}