package com.example.sk;

import android.util.Log;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;


public class SocketConnector {
    private static SocketConnector socketConnector;
    private Socket javaSocket;
    private int port = 1234;
    private String ip_address;
    private PrintWriter out;
    private BufferedReader in;
    private String uniqueId;
    private static final String TAG = "mymsg";

    public SocketConnector() throws IOException {
        socketConnector = this;
        ip_address = "";
    }

    public void setHost(String h) {
        ip_address = h;
    }

    public void connect(){
        try {
            javaSocket = new Socket();
            Log.d(TAG, javaSocket.toString());

            if(ip_address != "")
                ((Socket) javaSocket).connect(new InetSocketAddress(ip_address, port), 3000);
            else throw new Exception("ip address is null!");

            in = new BufferedReader(
                    new InputStreamReader(javaSocket.getInputStream()));
            out = new PrintWriter(javaSocket.getOutputStream(), true);
            Log.d(TAG, "connect finished");
        } catch (Exception e) {
            Log.d(TAG, "from connect: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void disconnect() {
        try {
            javaSocket.close();
            in.close();
            out.close();
        } catch (IOException e) {
            Log.d(TAG, e.getStackTrace().toString());
            e.printStackTrace();
        }
    }

    public void askForId() {
        try {
            connect();
            out.println("i");
            out.flush();
            uniqueId = in.readLine();
            System.out.println("U " + uniqueId);
            Log.d(TAG, "odebrano id " + uniqueId);
            disconnect();
        } catch (IOException e){
            e.printStackTrace();
            Log.d(TAG, e.getMessage());
        }
    }

    public void sendImg(String base64) {
        try {
            Log.d(TAG, base64.length() + "\n" + base64);
            connect();
            out.write("c\r\n");
            out.flush();

            out.write("%" + uniqueId + "\r\n");
            out.flush();

            out.write(String.format("%010d", base64.length() + 2) + "\r\n");
            out.flush();

            out.write(base64 + "\r\n");
            System.out.println(base64.length());
            out.flush();

            Log.d(TAG, "koncze wysyłanie");
            disconnect();
        } catch (Exception e) {
            Log.d(TAG, "there was a problem during sending img: " + e.getMessage());
        }
    }

    public static SocketConnector getInstance(){
        return socketConnector;
    }
}
