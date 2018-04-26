package com.example.massi.myapplication.ui

import android.app.ActionBar
import android.graphics.Color
import android.os.Bundle
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TableLayout
import android.widget.TableRow
import com.example.massi.myapplication.*
import kotlinx.android.synthetic.main.start_fragment_layout.*
import kotlinx.android.synthetic.main.start_fragment_layout.view.*
import java.util.logging.Logger

class SetupGridFragment : Fragment() {

    companion object {
        val LOG = Logger.getLogger(SetupGridFragment::class.java.name)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.setup_grid_fragment_layout, container, false)

        this.createTable(this.krobot.explorationMap.rows, this.krobot.explorationMap.columns)

        return view
    }

    private fun createTable(rows: Int, columns: Int) {
        val table : TableLayout = TableLayout(this.context)
        //the table layout needs to be put within a framelayout, hence we need to use LayoutParams of TableLayout
        val params : TableLayout.LayoutParams = TableLayout.LayoutParams(TableLayout.LayoutParams.WRAP_CONTENT, TableLayout.LayoutParams.WRAP_CONTENT)
        params.setMargins(5,5,5,5)

        for (row in 0..rows) {
            LOG.fine(String.format("Building row %d in table", row))
            val tableRow = TableRow(this.context)

            for (col in 0..columns) {
                LOG.fine(String.format("Building cell y=%d x=%d", row, col))
                val cell = ColorView(this.context, Color.BLACK)
                cell.layoutParams = params
                tableRow.addView(cell)
            }


            table.addView(tableRow)
        }
    }

}
