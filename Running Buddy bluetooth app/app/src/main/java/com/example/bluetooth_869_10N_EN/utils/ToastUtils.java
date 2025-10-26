package com.example.bluetooth_869_10N_EN.utils;

import static com.example.bluetooth_869_10N_EN.base.BaseApp.getPrivateInstance;

import android.content.Context;
import android.content.res.Resources;
import android.text.TextUtils;
import android.widget.Toast;

public class ToastUtils {
    public static boolean isShow = true;

    private static void toast(CharSequence message, int duration) {
        Context context = getPrivateInstance();
        if (context == null) return;
        Toast.makeText(context, message, duration).show();
    }

    /**
     * 短时间显示Toast
     *
     * @param message
     */
    public static void showShort(CharSequence message) {
        if (TextUtils.isEmpty(message)) {
            message = "";
        }
        if (isShow) {
            toast(message, Toast.LENGTH_SHORT);
        }
    }

    /**
     * 短时间显示Toast
     *
     * @param message
     */
    public static void showShort(int message) {
        if (isShow) {
            showShort(Resources.getSystem().getString(message));
        }
    }

    /**
     * 长时间显示Toast
     *
     * @param message
     */
    public static void showLong(CharSequence message) {
        if (TextUtils.isEmpty(message)) {
            message = "";
        }
        if (isShow) {
            toast(message, Toast.LENGTH_LONG);
        }
    }

    /**
     * 长时间显示Toast
     *
     * @param message
     */
    public static void showLong(int message) {
        if (isShow) {
            showLong(Resources.getSystem().getString(message));
        }
    }

}
