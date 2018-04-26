package com.example.massi.myapplication.ui

import android.os.Bundle
import android.support.v4.app.Fragment
import android.view.ViewGroup
import android.view.LayoutInflater
import android.view.View
import com.example.massi.myapplication.*
import kotlinx.android.synthetic.main.start_fragment_layout.*
import kotlinx.android.synthetic.main.start_fragment_layout.view.*
import java.util.logging.Logger


public class StartFragment : Fragment() {

    companion object {
        val LOG = Logger.getLogger(StartFragment::class.java.name)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.start_fragment_layout, container, false)

        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.connectRobot})
                .subscribe({e ->
                    //perform bluetooth connection
                    (this.activity as MainActivity).bluetoothManager.requestConnect()
                    //activate the waiting status
                    (this.activity as MainActivity).setWaiting(true)
                })

        this.krobot.readStream
                .filter({e -> e is BluetoothEvent && e.connectionSuccessful})
                .subscribe({e ->
                    LOG.info("we need to reactivate the main activity")
                    //the view has been reactivated
                    (this.activity as MainActivity).setWaiting(false)
                    (this.activity as MainActivity).changeMainPanel(SetupGridOptionsFragment())
                })

        view.connectRobot.setOnClickListener({_ -> this.krobot.writeStream.onNext(KrobotAppButtonPressed(view.connectRobot))})

        return view
    }

}