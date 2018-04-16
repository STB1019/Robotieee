package com.example.simone.robotieee;

/**
 * Created by simone on 15/03/18.
 */

import java.io.Serializable;

/**
 *
 {
    "version": "1.0",
    "world": {
        "rows": 3,
        "columns": 3,
        "cells": [
            { "y": 0, "x": 0, "entities": "G"},
            { "y": 0, "x": 1, "entities": "R"},
            { "y": 0, "x": 2, "entities": "U"},
            { "y": 1, "x": 0, "entities": ""},
            { "y": 1, "x": 1, "entities": "B"},
            { "y": 1, "x": 2, "entities": ""},
            { "y": 2, "x": 0, "entities": ""},
            { "y": 2, "x": 1, "entities": ""},
            { "y": 2, "x": 2, "entities": ""}
        ]
    }
 }
 */

public class JsonObjSend{

    private String version;
    private World world;
    private int solution_number;

    public JsonObjSend(String version, World world, int solNumb){
        this.version = version;
        this.solution_number = solNumb;
        this.world = world;
    }

    public void setVersion(String version) {

        this.version = version;

    }
}

class World{

    private int rows;
    private int columns;
    private Cells[] cells;

    public World(int rows, int columns, Cells[] cells){

        this.rows = rows;
        this.columns = columns;
        this.cells = cells;

    }
}


class Cells{

    int y;
    int x;

    String entities;

    Cells(int y, int x, String entities){

        this.y = y;
        this.x = x;
        this.entities = entities;

    }

}


