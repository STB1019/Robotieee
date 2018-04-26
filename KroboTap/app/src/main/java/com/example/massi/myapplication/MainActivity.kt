package com.example.massi.myapplication

import android.bluetooth.BluetoothManager
import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.app.Fragment
import android.transition.Visibility
import android.util.Log
import android.view.View
import android.view.View.GONE
import android.view.View.VISIBLE
import android.view.WindowManager
import android.widget.LinearLayout
import com.example.massi.myapplication.model.FakeBluetoothManager
import com.example.massi.myapplication.model.IBluetoothManager
import com.example.massi.myapplication.ui.SetupGridOptionsFragment
import com.example.massi.myapplication.ui.StartFragment
import io.reactivex.Observable
import io.reactivex.subjects.PublishSubject
import io.reactivex.subjects.Subject
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import java.util.*
import java.util.logging.Logger


/**
 * Main (and only) activity in the app
 */
class MainActivity : AppCompatActivity() {

    private var currentMainFragment: Fragment = StartFragment()
    public val bluetoothManager : IBluetoothManager = FakeBluetoothManager()
    public var isActivityWaiting : Boolean = false
        private set

    companion object {
        val LOG = Logger.getLogger(MainActivity::class.java.name)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        this.changeMainPanel(this.currentMainFragment ?: throw KroboTappException("invalid scenario"))
    }

    /**
     * Enable of disable the screen.
     *
     * Useful while waiting for some operation.
     *
     * The method is inspired from <a href="https://stackoverflow.com/a/10721034/1887602">here</a>
     *
     * @param[enable] true if you want to enable the screen, false otherwise
     */
    public fun setWaiting(isWaiting: Boolean) {
//        if (isWaiting) {
//            this.window.clearFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE)
//            this.main_activity_root.setBackgroundColor(Color.parseColor("#B0000000"))
//            this.main_activity_root.visibility = View.GONE
//            isActivityWaiting = true
//            this.krobot.writeStream.onNext(WaitingActivityEvent(this, true))
//        } else {
//            this.window.setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE, WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE)
//            this.main_activity_root.setBackgroundColor(Color.parseColor("#00000000"))
//            this.main_activity_root.visibility = View.VISIBLE
//            isActivityWaiting = false
//            this.krobot.writeStream.onNext(WaitingActivityEvent(this, false))
//
//        }
    }

    /**
     * Change the content of the #mainPanel. Within it there should be a fragment.
     */
    public fun changeMainPanel(newFragment: Fragment) {
        val fragmentManager = this.supportFragmentManager

            LOG.info(String.format("changing the main fragment to %s!", newFragment.javaClass.simpleName))

            val oldFragment = this.currentMainFragment
            this.currentMainFragment = newFragment
            fragmentManager
                .beginTransaction()
                .remove(oldFragment)
                .commit()
            LOG.info("removed fragment %s", oldFragment.javaClass.simpleName)

            fragmentManager
                .beginTransaction()
                .add(R.id.mainPanel, this.currentMainFragment)
                .commit()
        LOG.info("added fragment %s", this.currentMainFragment.javaClass.simpleName)

            LOG.info("main fragment changed")

    }
}
