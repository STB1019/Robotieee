package com.example.massi.myapplication.model

/**
 * Represents an AbstractGrid where each cell is a set of elements
 */
public open class AbstractItemGrid<T>(rows: Int, columns: Int, cellPopulator: (Point) -> MutableSet<T>) : AbstractGrid<MutableSet<T>>(rows, columns, cellPopulator) {

    public fun contains(p: Point, t: T) : Boolean {
        return t in this[p]
    }

    public open fun addContent(p: Point, t: T) {
        this[p].add(t)
    }

    public open fun removeContent(p: Point, t: T) {
        this[p].remove(t)
    }

    public open fun clearContent(p: Point) {
        this[p].clear()
    }

    public fun size(p: Point) : Int {
        return this[p].size
    }

    public fun iterator(p: Point): Iterable<T> {
        return this[p].asIterable()
    }

}