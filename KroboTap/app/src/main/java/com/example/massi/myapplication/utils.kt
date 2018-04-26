package com.example.massi.myapplication

import android.os.Bundle
import android.os.Trace
import android.support.v4.app.Fragment
import android.support.v7.app.AppCompatActivity
import com.example.massi.myapplication.model.AbstractRobotieeeMessage
import io.reactivex.Observable
import io.reactivex.subjects.PublishSubject
import io.reactivex.subjects.Subject
import java.lang.ref.WeakReference
import java.util.*

/**
 * An activity which automatically populates #GlobalVariables.mainActivity value
 */
//public open abstract class TraceableActivity: AppCompatActivity() {
//
//    override fun onStart() {
//        super.onStart()
//        GlobalVariables.mainActivity = this
//    }
//
//    override fun onRestart() {
//        super.onRestart()
//        GlobalVariables.mainActivity = this
//    }
//
//    override fun onStop() {
//        super.onStop()
//        GlobalVariables.mainActivity = null
//    }
//
//}

/**
 * Represents a runnaable which has the embedded stoppable operation and an internal event loop.
 *
 * Use #requestStop to start the stopping procedure of the thread
 *
 * @param[pollingTime] the time, in mmillisecond, to wait before executing #internalRun. If 0 or less, the thread won't wait any time before
 *  executing #internalRun
 */
public open abstract class StoppableRunnable(waitTime: Long): Runnable {

    private var requestStop = false
    private var waitTime = waitTime

    public abstract fun internalRun()

    override fun run() {
        while (true) {
            if (requestStop) {
                break
            }

            if (waitTime > 0) {
                Thread.sleep(waitTime)
            }
            this.internalRun()
        }
    }

    fun requestStop() {
        this.requestStop = true
    }
}