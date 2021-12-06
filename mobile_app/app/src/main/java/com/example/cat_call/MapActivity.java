package com.example.cat_call;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

public class MapActivity extends AppCompatActivity {

    private ImageView cat;
    private ConstraintLayout constraints;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map);
        Intent intent = getIntent();
        String room = intent.getStringExtra("room");
        cat = findViewById(R.id.cat);
        constraints = findViewById(R.id.constraints);
        if (room.equals("living room")) {
            changeConstraints(695, 435);
        } else if (room.equals("kitchen")) {
            changeConstraints(300, 975);
        } else if (room.equals("abed")) {
            changeConstraints(695, 975);
        } else if (room.equals("corridor")) {
            changeConstraints(1200, 860);
        } else if (room.equals("bathroom")) {
            changeConstraints(1230, 1100);
        } else if (room.equals("wbed")) {
            changeConstraints(1650, 550);
        } else if (room.equals("tbed")) {
            changeConstraints(1800, 1050);
        } else if (room.equals("unknown")) {
            cat.setVisibility(View.INVISIBLE);
        }
    }

    protected void changeConstraints(int x, int y) {
        ConstraintSet constraintSet = new ConstraintSet();
        constraintSet.clone(constraints);
        constraintSet.connect(R.id.cat, ConstraintSet.START, R.id.imageView, ConstraintSet.START, x);
        constraintSet.connect(R.id.cat, ConstraintSet.BOTTOM, R.id.imageView, ConstraintSet.BOTTOM, y);
        constraintSet.applyTo(constraints);
    }
}