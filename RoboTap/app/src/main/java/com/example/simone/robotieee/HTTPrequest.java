package com.example.simone.robotieee;

import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.google.gson.Gson;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import com.google.gson.GsonBuilder;

/**
 * Created by simone on 12/03/18.
 */

public class HTTPrequest {

    public String jsonObj;

    public MyJSONReply sendd(String uuu) throws IOException {

        MyJSONReply reply = null;

        URL url = new URL(uuu);
        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
        try {
            InputStream in = new BufferedInputStream(urlConnection.getInputStream());
            jsonObj = readStream(in);

            Gson gson = new Gson();

            reply = gson.fromJson(jsonObj, MyJSONReply.class);
        } finally {
            urlConnection.disconnect();
        }

        return reply;
    }

    /**
     * Manda un file al server e poi lo riceve con l'aggiunta di un id
     * @throws IOException
     */


    public MyJSONReply send(JsonObjSend jsonObjSend, String solution_url) throws IOException {

        URL url = null;
        MyJSONReply reply = null;
        //Scan scanAct;

        try {
            url = new URL(solution_url);
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }

        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
        urlConnection.setRequestProperty("Content-Type", "application/json");

        try {
            urlConnection.setDoOutput(true);
            urlConnection.setChunkedStreamingMode(0);

            OutputStream out = new BufferedOutputStream(urlConnection.getOutputStream());
            writeStream(out, jsonObjSend);

            try {
                InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                jsonObj = readStream(in);
            } catch (FileNotFoundException e){
                Log.d("Errore", "Nessun file ricevuto");
            }

            Gson gson = new Gson();

            reply = gson.fromJson(jsonObj, MyJSONReply.class);

        } catch (JSONException e) {

            e.printStackTrace();

        } finally {

            urlConnection.disconnect();

        }

        return reply;

    }

    /**
     * trasforma una stringa in bytes per mandarla al server
     * @param out
     * @throws IOException
     * @throws JSONException
     */

    private void writeStream(OutputStream out, JsonObjSend jsonObjSend) throws IOException, JSONException {

        Gson gson = new Gson();
        jsonObj = gson.toJson(jsonObjSend);
        Log.d("json", jsonObj);
        out.write(jsonObj.getBytes());
        out.flush();
    }

    private String readStream(InputStream is) {
        try {
            ByteArrayOutputStream bo = new ByteArrayOutputStream();
            int i = is.read();
            while(i != -1) {
                bo.write(i);
                i = is.read();
            }
            return bo.toString();
        } catch (IOException e) {
            return "";
        }
    }

}