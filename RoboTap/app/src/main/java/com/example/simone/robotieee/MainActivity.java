package com.example.simone.robotieee;

import android.app.VoiceInteractor;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    Button start, options, info;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        start = (Button) findViewById(R.id.Start);

        /*
        options = (Button) findViewById(R.id.Options);
        info = (Button) findViewById(R.id.Info);
        */

        start.setOnClickListener(this);
        /*
        options.setOnClickListener(this);
        info.setOnClickListener(this);
        */
    }


    @Override
    public void onClick(View view) {
        switch(view.getId()){

            case R.id.Start:

                Intent start = new Intent(MainActivity.this, SetMap.class);
                startActivity(start);
                break;

                /*
            case R.id.Options:

                Intent options = new Intent(MainActivity.this, Drive.class);
                startActivity(options);
                break;

            case R.id.Info:

                Intent info = new Intent(MainActivity.this, Info.class);
                startActivity(info);
                break;
                */

            default:

                Toast.makeText(getApplicationContext(),
                        "There are some problems",
                        Toast.LENGTH_SHORT).show();
                break;



        }
    }
}
