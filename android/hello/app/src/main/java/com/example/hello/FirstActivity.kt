package com.example.hello

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.Toast

class FirstActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.first_layout)
        val button1: Button = findViewById(R.id.button)
        button1.setOnClickListener{
            Toast.makeText(this,"Hello FFmpeg Principle", Toast.LENGTH_SHORT).show()
        }
    }
}