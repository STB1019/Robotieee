package com.example.massi.myapplication.ui

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.view.View

/**
 * Represents a view filled with a solid color
 */
class ColorView(context: Context, acolor: Int): View(context) {

    init {
        this.setBackgroundColor(acolor)
    }

    constructor(context: Context, color: Color) : this(context, color.toArgb())

    public var color : Int = acolor
        set(value) {
            color = value
            this.setBackgroundColor(color)
        }


}