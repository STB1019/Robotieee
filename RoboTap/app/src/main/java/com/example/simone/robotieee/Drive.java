package com.example.simone.robotieee;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

public class Drive extends AppCompatActivity implements View.OnClickListener {

    /**
     *Drive mode, you use it when you want to control the robot in real-time. There is also a button
     * that you could use anytime you want to switch to robot's real task
     */

    BluetoothModule blm;
    Button stop, go, right, left, retro;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_drive);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        stop = (Button) findViewById(R.id.stop);
        stop.setOnClickListener(this);

        go = (Button) findViewById(R.id.go);
        go.setOnClickListener(this);

        right = (Button) findViewById(R.id.right);
        right.setOnClickListener(this);

        left = (Button) findViewById(R.id.left);
        left.setOnClickListener(this);

        retro = (Button) findViewById(R.id.Scan);
        retro.setOnClickListener(this);

        blm = new BluetoothModule(null);
        blm.connetti();

    }

    @Override
    public void onClick(View view) {

        switch (view.getId()){

            case R.id.stop:

                goDown();
                break;

            case R.id.go:

                go();
                break;

            case R.id.right:

                goRight();
                break;

            case R.id.left:

                goLeft();
                break;

            case R.id.Scan:

                goToScan();
                break;

        }
    }

    void goDown(){

        blm.write("M#20");

    }

    void go(){

        blm.write("M#00");

    }

    void goRight(){

        blm.write("M#10");

    }

    void goLeft(){

        blm.write("M#30");

    }

    void goToScan(){

        /**
         * This button changes the activity to robot's real task
         */

        Intent scanAct = new Intent(Drive.this, SetMap.class);
        startActivity(scanAct);

    }

}
