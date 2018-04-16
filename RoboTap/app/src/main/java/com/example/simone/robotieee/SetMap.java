package com.example.simone.robotieee;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

/**
 * In this activity you set the dimension of the map;
 */

public class SetMap extends AppCompatActivity implements View.OnClickListener{

    Button setMap;
    EditText rows, columns, blockNumber, solution;

    Integer row, column, block, solutionNumber;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_set_map);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setMap = (Button) findViewById(R.id.SetMap);
        setMap.setOnClickListener(this);

        rows = (EditText) findViewById(R.id.Rows);
        columns = (EditText) findViewById(R.id.Columns);
        blockNumber = (EditText) findViewById(R.id.Block);
        solution = findViewById(R.id.Solution);

    }

    /**
     * Pressing the button you send the dimension to the next acitivity;
     * @param view
     */

    @Override
    public void onClick(View view) {

        try {

            row = Integer.parseInt(rows.getText().toString());
            column = Integer.parseInt(columns.getText().toString());
            block = Integer.parseInt(blockNumber.getText().toString());
            solutionNumber = Integer.parseInt(solution.getText().toString());

            Intent start = new Intent(SetMap.this, Options.class);
            start.putExtra("rows", row);
            start.putExtra("columns", column);
            start.putExtra("blockNumber", block);
            start.putExtra("solutionNumber", solutionNumber);
            startActivity(start);
        } catch (NumberFormatException e){

            Toast.makeText(this, "Uncorrect values", Toast.LENGTH_LONG).show();

        }


    }

}
