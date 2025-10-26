package com.example.bluetooth_869_10N_EN.utils;

import android.content.Context;
import android.os.Build;
import android.os.VibrationEffect;
import android.os.Vibrator;

import static com.example.bluetooth_869_10N_EN.base.BaseApp.getPrivateInstance;

public class VibratorUtil {

    private static Vibrator vibrator;

    public static void start() {
        Context context = getPrivateInstance();
        if (context == null) return;
        if (vibrator == null) {
            vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        }
        // 延迟0ms，振动100ms
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            VibrationEffect effect = VibrationEffect.createWaveform(new long[]{0, 100}, -1);
            vibrator.vibrate(effect);
        } else {
            vibrator.vibrate(new long[]{0, 100, 0, 0}, -1);
        }
    }

    public static void stop() {
        vibrator.cancel();
    }

}
