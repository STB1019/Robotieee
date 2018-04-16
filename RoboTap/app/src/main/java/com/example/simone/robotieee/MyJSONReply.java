package com.example.simone.robotieee;

/**
 * Created by simone on 06/04/18.
 */

public class MyJSONReply {
    private String version;
    public Action[] actions;

    public MyJSONReply(String version, Action[] actions) {
        this.version = version;
        this.actions = actions;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public Action getActions(int n) {
        return actions[n];
    }

    public void setActions(Action[] actions) {
        this.actions = actions;
    }
}

class Action {
    public String action;
    private String player;
    private Posizione start_pos;
    private Posizione end_pos;
    public String direction;

    public Action(String action, String player, Posizione start_pos, Posizione end_pos, String direction) {
        this.action = action;
        this.player = player;
        this.start_pos = start_pos;
        this.end_pos = end_pos;
        this.direction = direction;
    }

    public String getAction() {
        return action;
    }

    public void setAction(String action) {
        this.action = action;
    }

    public String getPlayer() {
        return player;
    }

    public void setPlayer(String player) {
        this.player = player;
    }

    public Posizione getStart_pos() {
        return start_pos;
    }

    public void setStart_pos(Posizione start_pos) {
        this.start_pos = start_pos;
    }

    public Posizione getEnd_pos() {
        return end_pos;
    }

    public void setEnd_pos(Posizione end_pos) {
        this.end_pos = end_pos;
    }

    public String getDirection() {
        return direction;
    }

    public void setDirection(String direction) {
        this.direction = direction;
    }
}

class Posizione {
    private int x;
    private int y;

    public Posizione(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }

    public int getY() {
        return y;
    }

    public void setY(int y) {
        this.y = y;
    }
}
