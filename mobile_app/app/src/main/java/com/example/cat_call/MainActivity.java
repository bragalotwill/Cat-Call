package com.example.cat_call;


import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.os.Parcelable;
import android.view.View;

import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.Buffer;

public class MainActivity extends AppCompatActivity {

    private boolean listenSocket = true;

    private Button toggle;
    private Button map;
    private TextView t_xPos;
    private TextView t_yPos;
    private TextView t_speed;
    private TextView t_volume;

    public float x, y;
    public String speed;
    public String volume;
    public boolean ready = false;

    private String room;

    private static Context context;

    public IOLooper looper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        context = getApplicationContext();

        toggle = findViewById(R.id.button);
        map = findViewById(R.id.map);
        t_xPos = findViewById(R.id.x);
        t_yPos = findViewById(R.id.y);
        t_speed = findViewById(R.id.speed);
        t_volume = findViewById(R.id.volume);

        float x = 0;
        float y = 0;
        String speed = "";
        String volume = "";

        toggle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                listenSocket = !listenSocket;
            }
        });

        map.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (ready) {
                    Intent intent = new Intent(context, MapActivity.class);
                    room = findRoom(x, y);
                    intent.putExtra("room", room);
                    startActivity(intent);
                }
            }
        });

        if (listenSocket) {
            looper = new IOLooper(this);

            looper.setListener(new IOLooper.OnDataListener() {
                @Override
                //retrieve x, y, speed, volume
                public void onData(IOLooper looper, String data, MainActivity main) {
                    System.out.println(data);
                    String[] dataArr = data.split(",");
                    if (dataArr.length == 4) {
                        main.x = Float.parseFloat(dataArr[0]);
                        main.y = Float.parseFloat(dataArr[1]);
                        main.speed = dataArr[2];
                        main.volume = dataArr[3];
                        main.ready = true;
                        t_xPos.setText("" + main.x);
                        t_yPos.setText("" + main.y);
                        t_speed.setText(speed);
                        t_volume.setText(volume);
                    }
                }
            });
            looper.start();
        }
    }
    protected String findRoom(float x, float y) {
        if (true) {
            return "living room";
        }
        return "unknown";
    }
}