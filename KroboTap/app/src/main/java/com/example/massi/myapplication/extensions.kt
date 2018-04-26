package com.example.massi.myapplication

import android.app.Activity
import android.content.Context
import android.support.v4.app.Fragment
import android.util.Log
import com.example.massi.myapplication.model.FakeBluetoothManager
import com.example.massi.myapplication.model.KrobotappModel
import io.reactivex.Observable
import io.reactivex.subjects.Subject
import java.util.logging.Logger

val Activity.krobot : KrobotappModel
    get() = GlobalVariables.model

val Fragment.krobot : KrobotappModel
    get() = GlobalVariables.model

val FakeBluetoothManager.krobot : KrobotappModel
    get() = GlobalVariables.model

/**
 * Clamp a function between the given min and max
 */
fun Int.clamp(min: Int, max: Int) : Int{
    if (this < min ) {
        return min
    }
    if (this > max) {
        return max
    }

    return this
}

fun Logger.info(format: String, vararg objs: Any) {
    this.info(String.format(format, objs))
}

fun Logger.fine(format: String, vararg objs: Any) {
    this.fine(String.format(format, objs))
}