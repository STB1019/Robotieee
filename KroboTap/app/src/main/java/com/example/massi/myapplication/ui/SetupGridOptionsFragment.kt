package com.example.massi.myapplication.ui

import android.content.Context
import android.os.Bundle
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.massi.myapplication.*
import com.example.massi.myapplication.model.ExplorationGrid
import kotlinx.android.synthetic.main.setup_grid_options_fragment_layout.*
import kotlinx.android.synthetic.main.setup_grid_options_fragment_layout.view.*
import java.util.logging.Logger

class SetupGridOptionsFragment : Fragment() {

    companion object {
        val LOG = Logger.getLogger(SetupGridOptionsFragment::class.java.name)
    }

    private var currentRows = 0
    set(value) {
        field = value
        this.rowsCurrentValueTextView.text = currentRows.toString()
    }

    private var currentColumns = 0
        set(value) {
            field = value
            this.columnsCurrentValueTextView.text = currentColumns.toString()
        }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.setup_grid_options_fragment_layout, container, false)

        LOG.info("we are inside the onCreateView of %s", this.javaClass.simpleName)

        view.rowsMinus.setOnClickListener({_ -> this.currentRows = (this.currentRows - 1).clamp(1, 10) })
        view.rowsPlus.setOnClickListener({_ -> this.currentRows = (this.currentRows + 1).clamp(1, 10) })

        view.columnsMinus.setOnClickListener({_ -> this.currentColumns = (this.currentColumns - 1).clamp(1, 10) })
        view.columnsPlus.setOnClickListener({_ -> this.currentColumns = (this.currentColumns + 1).clamp(1, 10) })

        this.krobot.readStream
                .filter({e -> e is KrobotAppButtonPressed && e.source == this.confirm})
                .subscribe({e ->
                    this.krobot.explorationMap = ExplorationGrid(rows = this.currentRows, columns = this.currentColumns)
                    (this.activity as MainActivity).changeMainPanel(SetupGridFragment())
                })


        view.confirm.setOnClickListener({_ -> this.krobot.writeStream.onNext(KrobotAppButtonPressed(this.confirm))})

        return view
    }

}