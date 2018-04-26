package com.example.massi.myapplication.model

public class Point(y:Int, x:Int) {

    var x = x
        public get
        public set
    var y = y
        public get
        public set

    /**
     * Sums the current point with another one
     *
     * @param[p] the other point we need to sum the current one with
     * @return a new instance containing the sum of the 2 points
     */
    operator fun plus(p: Point) : Point {
        return Point(this.y + p.y, this.x + p.x)
    }

    /**
     * Subtract from the current point another one
     *
     * @param[p] the other point we need to substract
     * @return a new instance containing <tt>this - p</tt>
     */
    operator fun minus(p: Point) : Point {
        return Point(this.y - p.y, this.x - p.x)
    }

    /**
     * Clamp a number
     *
     * @param[min] the minimum acceptable value
     * @param[x] the value we optionally need to clamp
     * @param[max] the maximum acceptable value
     * @return the number (possibly clamped)
     */
    private fun clamp(min: Int, x: Int, max:Int): Int {
        if (x < min) {
            return min
        }
        if (x > max) {
            return max
        }
        return x
    }

    /**
     * Clamp the current point
     *
     * @param[min] the point representing the minimum coordinates
     * @param[max] the point representing the maximum coordinates
     * @return a new point where each coordinate (y,x) has been clamped w.r.t. min and max
     */
    fun clamp(min:Point, max:Point) : Point {
        var p = Point(this.y, this.x)

        p.x = this.clamp(min.x, p.x, max.x)
        p.y = this.clamp(min.y, p.y, max.y)

        return p
    }

    override fun toString(): String {
        return String.format("<y=%d, x=%d>", y, x)
    }
}