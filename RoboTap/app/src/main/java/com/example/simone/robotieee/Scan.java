package com.example.simone.robotieee;

import android.content.Context;
import android.content.Intent;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.support.annotation.DrawableRes;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import com.google.gson.Gson;

import org.w3c.dom.Text;

import java.io.IOException;
import java.util.ArrayList;

/**
 * This is the main activity where you have 3 times. In the first one you press the button, you send the info to the exploration_solver
 * and you receive the moves to the robot;
 * once the exploration is finished you receive from the robot the position of the block and you can press again to send the new
 * information to the server.
 * After this you will receive other moves by the sokoban solver to complete the task;
 */


public class Scan extends AppCompatActivity implements View.OnClickListener{

    public String logText, posX, posY;

    static MyJSONReply command;

    Button scan;
    TextView wifiStat, batteryStat, logConsole, warningStat, blueStat;
    Drawable green, red, best, good, bad, worst;

    World world;
    Cells[] cells;
    static JsonObjSend jsonObjSend;
    static HTTPrequest request;
    BluetoothModule blm;
    BottonMap map;
    String url_solution;
    String url_exploration;

    static int cont, rows, columns, version, blockNumber, TOTAL_CELL, addedBlock, solutionNumber;

    static Boolean readData, created, explorationFinished, sokobanFinished, premuto;

    NetworkInfo mWifi;
    ConnectivityManager connManager;


    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scan);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        url_exploration = "http://192.168.4.1:5000/exploration_problem";
        url_solution = "http://192.168.4.1:5000/sokoban_problem";

        Intent rs = getIntent();
        map = (BottonMap) rs.getSerializableExtra("map"); //you receive the serialized map;
        rows = rs.getIntExtra("rows", rows);
        columns = rs.getIntExtra("columns", columns);
        blockNumber = rs.getIntExtra("blockNumber", blockNumber);
        solutionNumber = rs.getIntExtra("solutionNumber", solutionNumber);
        TOTAL_CELL = rows*columns;

        cells = new Cells[TOTAL_CELL];

        version = 0;
        addedBlock = 0;

        scan = (Button) findViewById(R.id.Scan);
        wifiStat = (TextView) findViewById(R.id.ServerStat);
        warningStat = (TextView) findViewById(R.id.WarningStat);
        blueStat = (TextView) findViewById(R.id.BlueStat);

        batteryStat = (TextView) findViewById(R.id.Battery);
        batteryStat.setVisibility(View.INVISIBLE);
        logConsole = (TextView) findViewById(R.id.logConsole);

        scan.setOnClickListener(this);

        green = getResources().getDrawable(R.drawable.green);
        red = getResources().getDrawable(R.drawable.red);

        best = getResources().getDrawable(R.drawable.best);
        good = getResources().getDrawable(R.drawable.good);
        bad = getResources().getDrawable(R.drawable.bad);
        worst = getResources().getDrawable(R.drawable.worst);

        setBattery(100);
        setWifiStat();

        request = new HTTPrequest();

        readData = true;
        created = false;
        explorationFinished = false;
        sokobanFinished = false;
        premuto = true;

        blm = new BluetoothModule(this);
        blm.connetti();

        //scan.setEnabled(false);



    }

    @Override
    public void onClick(View view) {

        scan.setEnabled(false);

        new Thread(new Runnable() {

            public void run() {

                cont = 0;
                version = 0;

                if (!explorationFinished) {

                    firstPhase();


                } else if (explorationFinished){

                    sendStateChange();
                    setButtonFalse();
                    secondPhase();

                }

            }
        }).start();

    }

    public void setBluetooth(){

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                blueStat.setBackground(green);
            }
        });

    }

    public void setButtonTrue(){

        runOnUiThread(new Runnable() {
            @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
            @Override
            public void run() {
                scan.setEnabled(true);
                scan.setBackgroundTintList(ColorStateList.valueOf(Color.RED));
            }
        });

    }

    public void setButtonFalse(){

        runOnUiThread(new Runnable() {
            @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
            @Override
            public void run() {
                scan.setEnabled(false);
                scan.setBackgroundTintList(ColorStateList.valueOf(Color.LTGRAY));
            }
        });

    }

    void createJsonForExploration(){

        int cont = 0;

        for (int y = 0; y < rows; y++){
            for (int x = 0; x < columns; x++){

                String ent = map.botton[y][x].attr;
                cells[cont] = new Cells(y, x, ent);

                cont++;

            }
        }

        world = new World(rows, columns, cells);
        jsonObjSend = new JsonObjSend("1.0", world, solutionNumber);

        premuto = true;

    }

    /**
     * This method find and change the entities in a determinate cell;
     * @param
     * @param _x
     * @param _y
     */

    public void findAndChangeCellEntities(int _x, int _y, String ent){

        int number;

        for (int num = 0; num < TOTAL_CELL; num++){

            if (this.cells[num].entities.equals("RD")){

                this.cells[num].entities = "D";

            } else if (this.cells[num].entities.equals("R")){
                this.cells[num].entities = "V";
            }

        }

        for (int num = 0; num < TOTAL_CELL; num++){

            if ((this.cells[num].x == _x) && (this.cells[num].y == _y)){

                if (!(this.cells[num].entities.equals("D")) && !(this.cells[num].entities.equals("G")) && !(this.cells[num].entities.equals("B"))) {

                    this.cells[num].entities = ent;
                    if (ent.equals("B")){
                        addedBlock++;
                    }
                }

            }

        }

    }

    public void beforeSokobanSend(){

        for (int m = 0; m < TOTAL_CELL; m++){

            if (this.cells[m].entities.equals("V")){
                this.cells[m].entities = "";
            }

        }

    }

    JsonObjSend updateJsonObject(){

        jsonObjSend = new JsonObjSend("1.0", world, solutionNumber);

        return jsonObjSend;

    }

    public void setWifiStat(){

        connManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

        if (mWifi.isConnected()) {
            wifiStat.setBackground(green);
        } else {
            wifiStat.setBackground(red);
        }

    }

    public void updateLogConsole(String message){

        logConsole.setText(message + "\n" + logConsole.getText());

    }

    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
    public void setBattery(int battery){

        /**
         * This function receive battery status and set it for the user
         */

        float r = battery;

        if (r<=25){
            batteryStat.setBackground(worst);
        } else if (r<=50){
            batteryStat.setBackground(bad);
        } else if (r<=75){
            batteryStat.setBackground(good);
        } else if (r<=100){
            batteryStat.setBackground(best);
        }

    }

    public void firstPhase(){

        createJsonForExploration();

        try {
            setButtonFalse();

            command = request.send(jsonObjSend, url_exploration);
                    //request.send(jsonObjSend, url_exploration);//Here you send the jsonObj with the map to the server to plan the explanation;

            if (command != null) {
                created = true;//here you set that he received the list of moves;
            }

            //When the map is created and sent to the server;
            cont = 0;

            if (created) {

                if (addedBlock < blockNumber) {

                    cont = sendBunchMoves(cont);

                }
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void secondPhase(){

        cont = 0;

        /*
        try {

            command = request.send(jsonObjSend, url_solution);

        } catch (IOException e) {

            e.printStackTrace();

        }
        */

    }

    public void changeConsoleTextOnUi(final String message){

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                    updateLogConsole(message);//Here are shown the list of moves;
            }
        });

    }

    public void changeStat(){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                scan.setText("Start Task");//Here are shown the list of moves;
            }
        });
    }

    public int sendBunchMoves(int cont){

        char com;

        for (int bunch = 0; bunch < 10; bunch++) {

            if (cont < command.actions.length) {

                switch (command.actions[cont].direction) {

                    case "dir-down":
                        com = '2';
                        break;

                    case "dir-right":
                        com = '1';
                        break;

                    case "dir-left":
                        com = '3';
                        break;

                    case "dir-up":
                        com = '0';
                        break;

                    default:
                        com = 'h';
                        break;

                }

                blm.write(""+Dictionary.MOVEMENT+Dictionary.SEPARATOR+com+0);
                cont++;

            } else {

                blm.write(""+Dictionary.END+Dictionary.SEPARATOR);

            }

        }

        return cont;

    }

    public void sendStateChange(){

        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");
        blm.write("S#E");

    }

    public int sendBunchMovesSokoban(int cont){

            char com, type;

            for (int bunch = 0; bunch < 10; bunch++) {

                if (cont < command.actions.length) {
                    Log.d("lunghezza", ""+command.actions.length);

                    switch (command.actions[cont].direction) {

                        case "dir-down":
                            com = '2';
                            break;

                        case "dir-right":
                            com = '1';
                            break;

                        case "dir-left":
                            com = '3';
                            break;

                        case "dir-up":
                            com = '0';
                            break;

                        default:
                            com = 'h';
                            break;

                    }

                    if (command.actions[cont].action.equals("move")) {

                        type = '0';

                    } else {

                        type = '1';

                    }

                    blm.write("" + Dictionary.MOVEMENT + Dictionary.SEPARATOR + com + type);
                    cont++;

                } else {

                    blm.write(""+Dictionary.END+Dictionary.SEPARATOR);

                }

        }

        return cont;

    }

    @Override
    public void onStop() {

        blm.chiudiConnessione();
        super.onStop();

    }
}