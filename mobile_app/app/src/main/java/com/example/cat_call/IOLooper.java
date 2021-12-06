package com.example.cat_call;

import android.app.Activity;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.Buffer;

// help from https://stackoverflow.com/questions/28164085/android-keep-socket-open-to-read-message-from-server
public class IOLooper extends Thread{

    MainActivity main;
    public IOLooper(MainActivity main) {
        this.main = main;
    }

    public interface OnDataListener {
        public void onData(IOLooper looper, String data, MainActivity main);
    }

    OnDataListener listener = null;


    @Override
    public void run() {
        try {
            System.out.println("Connecting...");
            Socket socket = new Socket("192.168.0.112", 50000);
            System.out.println("Connected");
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String line;
            while (!Thread.currentThread().isInterrupted()) {
                line = in.readLine();
                if (line!= null && line != "") {
                    line = line.trim();
                    listener.onData(this, line, this.main);
                }
                Thread.sleep(1);
            }
            socket.close();
            in.close();
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error!");
            System.exit(1);
        }
    }

    public void setListener(OnDataListener listener) {
        this.listener = listener;
    }
}
