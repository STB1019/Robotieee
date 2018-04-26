package com.example.massi.myapplication.model

import com.example.massi.myapplication.StoppableRunnable
import io.reactivex.Observable
import io.reactivex.subjects.PublishSubject
import io.reactivex.subjects.Subject
import java.util.*
import kotlin.collections.ArrayList

private class ToRobotThread(commandFIFOQueue: Queue<AbstractRobotieeeMessage>, interCommandTime: Long) : StoppableRunnable(interCommandTime) {

    private val commandFIFOQueue = commandFIFOQueue
    private val _sendCommandStream: PublishSubject<AbstractRobotieeeMessage> = PublishSubject.create()
    public val sendCommandStream : Observable<AbstractRobotieeeMessage> get() = _sendCommandStream

    override fun internalRun() {
        val cmd = commandFIFOQueue.poll() ?: return

        //TODO send to bluetooth
        _sendCommandStream.onNext(cmd)

    }

}

/**
 * A runnable which listen on the bluetooth channel waiting for data from the bluetooth
 *
 * @param[pollingTime] the time, in milliseconds to wait in order to fetch data from the bluetooth
 */
private class FromRobotRoutine(commandFIFOQueue: Queue<AbstractRobotieeeMessage>, pollingTime: Long): StoppableRunnable(pollingTime) {

    private val commandFIFOQueue = commandFIFOQueue
    private val _receivedCommandStream: PublishSubject<AbstractRobotieeeMessage> = PublishSubject.create()
    public val receivedCommandStream : Observable<AbstractRobotieeeMessage> get() = _receivedCommandStream

    override fun internalRun() {
        val cmd = commandFIFOQueue.poll() ?: return
        _receivedCommandStream.onNext(cmd)
    }
}