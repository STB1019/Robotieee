package com.example.massi.myapplication

import android.support.v7.app.AppCompatActivity
import io.reactivex.Observable
import io.reactivex.subjects.PublishSubject
import io.reactivex.subjects.Subject

public open abstract class KrobotAppEvent(source: Any, t: Type) {

    val source = source
    val type = t

    public enum class Type {
        BUTTON_PRESSED,
        BLUETOOTH_EVENT,
        WAITING_EVENT,
    }

}

public class KrobotAppButtonPressed(source: Any) : KrobotAppEvent(source, Type.BUTTON_PRESSED) {

}

public class WaitingActivityEvent(source: Any, isWaiting: Boolean) : KrobotAppEvent(source, Type.WAITING_EVENT) {
    val isWaiting = isWaiting
}

public class BluetoothEvent(source: Any, direction: Direction, message: String?, connectionSuccessful: Boolean, disconnectionSuccessful: Boolean, disconnectionUnexpected: Boolean) : KrobotAppEvent(source, KrobotAppEvent.Type.BLUETOOTH_EVENT){

    public enum class Direction {
        SENT,
        RECEIVED
    }

    companion object {

        fun buildConnectionSuccessful(source: Any) : BluetoothEvent {
            return BluetoothEvent(source, Direction.SENT, "", true, false, false)
        }

        fun buildDisconnectionSuccessful(source: Any) : BluetoothEvent {
            return BluetoothEvent(source, Direction.SENT, "", false, true, false)
        }

        fun buildDisconnectionUnexpected(source: Any) : BluetoothEvent {
            return BluetoothEvent(source, Direction.SENT, "", false, false, true)
        }

        fun buildMessageReceived(source: Any, msg: String) : BluetoothEvent {
            return BluetoothEvent(source, Direction.RECEIVED, msg, false, false, false)
        }

        fun buildMessageSent(source: Any, msg: String) : BluetoothEvent {
            return BluetoothEvent(source, Direction.SENT, msg, false, false, false)
        }
    }

    public val connectionSuccessful = connectionSuccessful
    public val disconnectionSuccessful = disconnectionSuccessful
    public val direction = direction
    public val message = message
    public val disconnectionUnexpected = disconnectionUnexpected

}