package com.example.massi.myapplication.model

import com.example.massi.myapplication.BluetoothEvent
import com.example.massi.myapplication.krobot

interface IBluetoothManager {

    /**
     * Request to perform a connection
     *
     * When the connection has been establish a #BluetoothEvent with connection successful needs to be put on the stream
     *
     * @param[stayConnected] if true we will automatically reconnect if the channel collapse
     */
    fun requestConnect(stayConnected : Boolean = true)

    /**
     * Request the disconnection from the bluetooth device
     *
     * When the connection has been establish a #BluetoothEvent with disconnection successful needs to be put on the stream
     */
    fun requestDisconnect()
}

/**
 * The class mimics a bluetooth connection
 */
class FakeBluetoothManager : IBluetoothManager {

    override fun requestConnect(stayConnected : Boolean) {
        this.krobot.writeStream.onNext(BluetoothEvent.buildConnectionSuccessful(this))
    }

    override fun requestDisconnect() {
        this.krobot.writeStream.onNext(BluetoothEvent.buildDisconnectionSuccessful(this))
    }

}