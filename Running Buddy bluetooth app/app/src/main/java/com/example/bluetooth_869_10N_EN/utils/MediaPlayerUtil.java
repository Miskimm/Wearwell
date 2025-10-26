package com.example.bluetooth_869_10N_EN.utils;

import android.content.Context;
import android.media.MediaPlayer;

import static com.example.bluetooth_869_10N_EN.base.BaseApp.getPrivateInstance;

public class MediaPlayerUtil {

    private static MediaPlayer mediaPlayer;

    public static void start(int res) {
        Context context = getPrivateInstance();
        if (context == null) return;
        mediaPlayer = MediaPlayer.create(context, res);
        mediaPlayer.start();
    }

    public static void stop() {
        if (mediaPlayer != null) {
            mediaPlayer.stop();
        }
        mediaPlayer = null;
    }

}
