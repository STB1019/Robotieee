package com.example.simone.robotieee;

import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.widget.Button;

import java.io.Serializable;

/**
 * Created by simone on 06/04/18.
 *
 * This class is used to link a button with his corresponding attribute, that is selected by clicking a button;
 *
 */


@SuppressWarnings("serial")
public class BottonMap implements Serializable{

    /**
     * This class contains all the buttons with their corresponding attribute;
     */

    Botton[][] botton;


    public BottonMap(int rows, int columns){

        botton = new Botton[rows][columns];

        for (int i = 0; i < rows; i++){
            for (int m = 0; m < columns; m++){

                botton[i][m] = new Botton();

            }
        }

    }

    public class Botton implements Serializable{

        /**
         * This class contains the object button, the attribute and the method used to select the button's color;
         */

        public String attr;
        public transient Button button;

        public Botton(){

            attr = "";

        }

        @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
        public void changeStatus(Botton button){

            switch (button.attr){

                case "":
                    button.button.setBackgroundTintList(ColorStateList.valueOf(Color.BLACK));
                    button.attr = "RD";
                    break;

                case "RD":
                    button.button.setBackgroundTintList(ColorStateList.valueOf(Color.RED));
                    button.attr = "U";
                    break;

                case "U":
                    button.button.setBackgroundTintList(ColorStateList.valueOf(Color.GREEN));
                    button.attr = "G";
                    break;

                case "G":
                    button.button.setBackgroundTintList(ColorStateList.valueOf(Color.LTGRAY));
                    button.attr = "";
                    break;

                default:
                    break;


            }

        }

    }

}
