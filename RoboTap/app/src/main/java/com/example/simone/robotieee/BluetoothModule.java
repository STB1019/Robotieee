package com.example.simone.robotieee;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.Set;
import java.util.UUID;

public class BluetoothModule {

    public static final int MAX_ERRORI_CONNESSIONE = 3;
    public static final int MILLIS_SLEEP_RICONNESSIONE = 1500;
    public static final String DEVICE_NAME = "HC-06";
    public static final String DEVICE_UUID = "00001101-0000-1000-8000-00805F9B34FB";

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothDevice bluetoothDevice;
    public BluetoothSocket bluetoothSocket;
    private OutputStream btOut;
    private PrintStream btOutBuffered;
    public UUID myUUID;

    private ConnectThread connectThread;
    private ListenThread listenThread;

    private long socketVersion;
    private int erroriConnessione;

    public Scan scanActivity;

    /**
     * Costruttore, necessario per l'utilizzo della classe.
     * @param scanActivity l'oggetto corrispondente all'Activity Scan corrente,
     *                     necessario per richiamarne i metodi.
     */
    public BluetoothModule (Scan scanActivity) {
        this.scanActivity = scanActivity;
    }


    public long getSocketVersion () {
        return socketVersion;
    }
    public void setSocketVersion (long socketVersion) {
        this.socketVersion = socketVersion;
    }

    /**
     * Avvia la connessione Bluetooth:<br>
     * - cerca tra i dispositivi appaiati quello con nome {@link #DEVICE_NAME DEVICE_NAME};<br>
     * - avvia il {@link ConnectThread}, che cerca di instaurare una connessione al robot come client;<br>
     * - se la connessione avviene con successo, il {@link ConnectThread} termina e viene avviato
     * il {@link ListenThread}, che si occupa della ricezione di messaggi dal canale Bluetooth.
     */
    public void connetti () {
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            log ("non supporti bluetooth???", false);
            log ("ERRORE: BLUETOOTH NON SUPPORTATO!", true);
            return;
        }

        myUUID = UUID.fromString (DEVICE_UUID);

        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();

        if (pairedDevices.size() > 0) {
            for (BluetoothDevice device : pairedDevices) {
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address
                log ("device collegati: " + deviceName, false);
                if (deviceName.equals (DEVICE_NAME)) {
                    bluetoothDevice = device;
                    // prova a connetterti come client
                    log ("provo a collegarmi come client", false);
                    connectThread = new ConnectThread(this, device);
                    connectThread.start();
                    break;
                }
                log ("ERRORE: DEVI PRIMA COLLEGARTI AL ROBOT!", true);
            }
        }
        else {
            log ("nessun dispositivo bluetooth accoppiato!", false);
            log ("ERRORE: DEVI PRIMA COLLEGARTI AL ROBOT!", true);
            log ("Hai attivato il Bluetooth?", true);
        }
    }

    /**
     * Invia un messaggio testuale sul canale Bluetooth e svuota il buffer in uscita.
     * Questo metodo non gestisce l'eventuale risposta.
     * @param testo il messaggio da inviare.
     */

    public void write (String testo) {
        btOutBuffered.print (testo);
        btOutBuffered.flush ();
        log ("INVIO " + testo, true);
    }

    public void sonoConnesso (long socketVersion) {
        log ("ok, socket connesso", false);
        log ("Connesso.", true);

        listenThread = new ListenThread(this, bluetoothSocket, socketVersion);
        listenThread.start();
        scanActivity.setBluetooth();


        try {
            btOut = bluetoothSocket.getOutputStream();
            btOutBuffered = new PrintStream (btOut);
        } catch (IOException e) {
            log ("errore nella creazione dell'output stream", false);
            riconnetti (socketVersion);
            return;
        }
        log ("Canale di output aperto.", true);
        erroriConnessione = 0;
    }

    /**
     * Chiude la connessione Bluetooth attualmente aperta - presumibilmente in stato di errore - e
     * prova a riavviarla, fino ad un massimo di {@link #MAX_ERRORI_CONNESSIONE MAX_ERRORI_CONNESSIONE}
     * tentativi consecutivi. Gestisce anche il riavvio dei vari thread.<br>
     *     Questo metodo viene richiamato automaticamente in caso di vari problemi di connessione.
     * @param socketVersion l'identificativo della connessione in errore.
     */

    public synchronized void riconnetti (long socketVersion) {
        // ignora le chiamate ridondanti
        if (socketVersion < this.getSocketVersion())
            return;
        // registra evento
        erroriConnessione ++;
        // verifica il conteggio degli errori
        if (erroriConnessione == MAX_ERRORI_CONNESSIONE + 1) {
            log ("troppi errori di connessione!", false);
            log ("ERRORE: RICONNESSIONE FALLITA!", true);
            tornaAlloStatoIniziale ();
            return;
        }
        else if (erroriConnessione > MAX_ERRORI_CONNESSIONE + 1)
            return;
        log ("riconnessione richiesta", false);
        log ("ERRORE! RICONNESSIONE...", true);
        chiudiConnessione();
        try {
            Thread.sleep (MILLIS_SLEEP_RICONNESSIONE);
        } catch (InterruptedException e) {
            log ("qualcuno ha svegliato lo sleep di riconnessione?!", false);
        }
        // lancia connect thread
        connectThread = new ConnectThread(this, bluetoothDevice);
        connectThread.start();
    }

    private void tornaAlloStatoIniziale () {
        erroriConnessione = 0;
    }

    /**
     * Chiude la connessione Bluetooth correntemente aperta e arresta il
     * {@link ListenThread}.
     */

    public void chiudiConnessione () {
        // ferma listen thread
        if (listenThread != null) {
            listenThread.termina ();
            listenThread.interrupt();
        }
        // prova a chiudere socket
        if (bluetoothSocket != null) {
            try {
                bluetoothSocket.close();
                log ("socket chiuso correttamente", false);
            } catch (IOException e) {
                log ("chiusura del socket fallita", false);
            }
        }
    }

    /**
     * Registra ed eventualmente visualizza un messaggio testuale.
     * @param testo il testo da registrare ed eventualmente visualizzare.
     * @param visualizza definisce se il testo debba essere riportato sull'interfaccia
     *                   dell'applicazione (true), oppure solo nel Logcat (false).
     */
    public void log (String testo, boolean visualizza) {
        Log.d ("BluetoothModule", testo);
        if (visualizza && (scanActivity != null) && (scanActivity.logConsole != null)) {
            scanActivity.changeConsoleTextOnUi(testo);
        }
    }
}

/**
 * Il ConnectThread viene istanziato e avviato al momento dell'apertura della connessione
 * Bluetooth; ha i seguenti compiti:<br>
 * - avviare la connessione BluetoothSocket;<br>
 * - aprire gli stream in entrata e in uscita;<br>
 * - avviare il {@link ListenThread};<br>
 * Successivamente, termina.
 */
class ConnectThread extends Thread {
    private BluetoothModule bluetoothModule;
    private long socketVersion;
    private boolean errore = false;

    public ConnectThread (BluetoothModule bluetoothModule, BluetoothDevice device) {
        this.bluetoothModule = bluetoothModule;
        this.socketVersion = System.currentTimeMillis();
        bluetoothModule.setSocketVersion (socketVersion);
        try {
            // MY_UUID is the app's DEVICE_UUID string, also used in the server code.
            bluetoothModule.log ("uuid: " + bluetoothModule.myUUID.toString(), false);
            bluetoothModule.bluetoothSocket = device.createRfcommSocketToServiceRecord (bluetoothModule.myUUID);
        } catch (IOException e) {
            bluetoothModule.log ("ConnectThread: Socket's create() method failed", false);
            bluetoothModule.log ("ERRORE: BLUETOOTH NON DISPONIBILE!", true);
            errore = true;
        }
    }

    public void run() {
        if (errore)
            return;
        try {
            // Connect to the remote device through the socket. This call blocks
            // until it succeeds or throws an exception.
            bluetoothModule.log ("connectThread: pre connessione", false);
            bluetoothModule.bluetoothSocket.connect();
            bluetoothModule.log ("connectThread: post connessione", false);
        } catch (IOException connectException) {
            bluetoothModule.log ("connectThread: non sono riuscito a connettermi", false);
            bluetoothModule.riconnetti (socketVersion);
            return;
        }
        // connesso
        if (bluetoothModule.scanActivity != null) {
            bluetoothModule.sonoConnesso(socketVersion);
        }
    }
}

/**
 * Il ListenThread, una volta avviato, resta in ascolto sullo stream di ingresso del
 * canale Bluetooth, riconosce i messaggi in entrata in base al dizionario e richiama
 * i dovuti metodi.<br>
 * E' l'unica parte del BluetoothModule che riceve messaggi via Bluetooth.
 */
class ListenThread extends Thread {
    private BluetoothModule bluetoothModule;
    private InputStream btIn;
    private BufferedReader btInBuffered;
    private boolean termina = false;
    private long socketVersion;

    public ListenThread(BluetoothModule bluetoothModule, BluetoothSocket socket, long socketVersion) {
        this.bluetoothModule = bluetoothModule;
        this.socketVersion = socketVersion;

        try {
            btIn = socket.getInputStream();
            btInBuffered = new BufferedReader(new InputStreamReader(btIn));
        } catch (IOException e) {
            bluetoothModule.log("ListenThread: socket non connesso", false);
            bluetoothModule.riconnetti(socketVersion);
        }
        bluetoothModule.log("Canale di input aperto.", true);
    }

    public void run() {
        bluetoothModule.log("ListenThread avviato.", true);
        char action;
        while (!termina) {
            try {
                bluetoothModule.log("ListenThread: ascolto", false);
                action = (char) btIn.read();
                switch (action) {
                    case Dictionary.LOCATION:
                        ricevutoLocation();
                        break;
                    case Dictionary.WARNING:
                        ricevutoWarning();
                        break;
                    case Dictionary.DONE:
                        Log.d("entrato nel case", "v");
                        ricevutoDone();
                        break;
                    default:
                        bluetoothModule.log("Messaggio sconosciuto da robot.", true);
                }
                bluetoothModule.log("Robot: messaggio di tipo " + action, false);
            } catch (IOException e) {
                if (termina)
                    return;
                bluetoothModule.log("ListenThread: Input stream disconnesso!", false);
                bluetoothModule.riconnetti(socketVersion);
                return;
            }
        }
    }

    private void ricevutoLocation() throws IOException {
        char[] payload = readPayload(Dictionary.LOCATION_PAYLOAD_DIM);
        int xBlocco = -1;
        int yBlocco = -1;
        int xRobot = Integer.parseInt("" + payload[2]);
        int yRobot = Integer.parseInt("" + payload[3]);
        boolean blockFound = true;

        try {
            xBlocco = Integer.parseInt("" + payload[0]);
            yBlocco = Integer.parseInt("" + payload[1]);

            bluetoothModule.scanActivity.findAndChangeCellEntities(xBlocco, yBlocco, "B");
            bluetoothModule.log("Block found: " + xBlocco + " " + yBlocco, true);

        }
        catch (NumberFormatException exception) {

            blockFound = false;

        }

        if (!blockFound) {

            bluetoothModule.scanActivity.findAndChangeCellEntities(xRobot, yRobot, "V");
            bluetoothModule.log("v "+xRobot+" "+yRobot,true);

        } else {

            bluetoothModule.scanActivity.findAndChangeCellEntities(xRobot, yRobot, "R");
            bluetoothModule.scanActivity.jsonObjSend = bluetoothModule.scanActivity.updateJsonObject();

            if (bluetoothModule.scanActivity.addedBlock == bluetoothModule.scanActivity.blockNumber) {
                bluetoothModule.scanActivity.explorationFinished = true;
                bluetoothModule.scanActivity.changeStat();
                bluetoothModule.scanActivity.setButtonTrue();
                bluetoothModule.scanActivity.beforeSokobanSend();
                bluetoothModule.scanActivity.command = bluetoothModule.scanActivity.request.send(bluetoothModule.scanActivity.jsonObjSend, bluetoothModule.scanActivity.url_solution);

            } else {

                bluetoothModule.scanActivity.command = bluetoothModule.scanActivity.request.send(bluetoothModule.scanActivity.jsonObjSend, bluetoothModule.scanActivity.url_exploration);
                bluetoothModule.scanActivity.cont = 0;
                bluetoothModule.scanActivity.cont = bluetoothModule.scanActivity.sendBunchMoves(bluetoothModule.scanActivity.cont);

            }
        }

    }

    private void ricevutoWarning() throws IOException {
        char[] payload = readPayload(Dictionary.WARNING_PAYLOAD_DIM);
        int nIstruzione = Integer.parseInt("" + payload[0]);
        if (bluetoothModule.scanActivity.explorationFinished) {
            if ((bluetoothModule.scanActivity.cont - (10-nIstruzione)) < 0){
                bluetoothModule.scanActivity.sendStateChange();
            } else {
                bluetoothModule.scanActivity.cont = bluetoothModule.scanActivity.sendBunchMovesSokoban(bluetoothModule.scanActivity.cont - (10 - nIstruzione));
            }
        } else {
            bluetoothModule.scanActivity.cont = bluetoothModule.scanActivity.sendBunchMoves(bluetoothModule.scanActivity.cont - (10 - nIstruzione));
        }
        //bluetoothModule.scanActivity.warningStat.setBackground(bluetoothModule.scanActivity.red);
    }

    private void ricevutoDone() throws IOException {
        char[] payload = readPayload(Dictionary.DONE_PAYLOAD_DIM);
        //int batteryLevel = Integer.parseInt(new String(payload));
        //synchronized (bluetoothModule) {
            if (bluetoothModule.scanActivity.explorationFinished) {
                Log.d("lunghezza",""+bluetoothModule.scanActivity.command.toString());
                if (bluetoothModule.scanActivity.cont >= bluetoothModule.scanActivity.command.actions.length) {

                } else {
                    bluetoothModule.scanActivity.cont = bluetoothModule.scanActivity.sendBunchMovesSokoban(bluetoothModule.scanActivity.cont);
                }
            } else {

                if (bluetoothModule.scanActivity.cont < bluetoothModule.scanActivity.command.actions.length) {
                    bluetoothModule.scanActivity.cont = bluetoothModule.scanActivity.sendBunchMoves(bluetoothModule.scanActivity.cont);
                }

            }
        //}

    //bluetoothModule.scanActivity.setBattery(batteryLevel);
        bluetoothModule.scanActivity.setWifiStat();
    }

    private char[] readPayload (int payloadDim) throws IOException {
        char separator = (char) btIn.read();
        char[] payload = new char[payloadDim];
        for (int i = 0; i < payloadDim; i++)
            payload[i] = (char) btIn.read();
        return payload;
    }

    public void termina () {
        termina = true;
    }
}
/*
    Per Simone:
    Per utilizzare la classe, istanzia un oggetto BluetoothModule passando al costruttore
    un this (oggetto Scan), poi richiama il metodo connetti().
    L'oggetto Scan mi serve per chiamare i tuoi metodi, quando ricevo messaggi dal robot.
    Per mandare messaggi al robot utilizza il metodo write (String testo).

    Per ricevere messaggi dal robot bisogna modificare la parte dello switch-case nel
    thread ListenThread. Posso farlo io, l'importante è che implementi dei metodi tipo
    quelli scritti nei commenti.

    Il metodo chiudiConnessione() andrebbe utilizzato nell'onStop() dell'activity Scan e/o
    su eventuali tasti per chiudere la connessione.

    Non dovresti avere bisogno degli altri metodi (in caso chiedimi prima di usarli).
 */