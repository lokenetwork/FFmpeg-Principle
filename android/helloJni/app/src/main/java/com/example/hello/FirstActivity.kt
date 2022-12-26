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
            Toast.makeText(this,"Hello FFmpeg Principle " + ffmpegVersion(),
                Toast.LENGTH_SHORT).show()
        }
    }

    /**
     * A native method that is implemented by the 'hellojni' native library,
     * which is packaged with this application.
     */
    external fun ffmpegVersion(): String

    companion object {
        // Used to load the 'hellojni' library on application startup.
        init {
            System.loadLibrary("hello")
            System.loadLibrary("avformat")
            System.loadLibrary("avcodec")

        }
    }
}