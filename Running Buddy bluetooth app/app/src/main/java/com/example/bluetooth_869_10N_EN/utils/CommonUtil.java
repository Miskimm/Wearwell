package com.example.bluetooth_869_10N_EN.utils;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Point;
import android.location.LocationManager;
import android.net.Uri;
import android.os.Build;
import android.os.IBinder;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;

import java.lang.reflect.Field;
import java.net.NetworkInterface;
import java.net.SocketException;

import static com.example.bluetooth_869_10N_EN.base.BaseApp.getPrivateInstance;

import androidx.annotation.NonNull;

public class CommonUtil {

    public static boolean checkGPSIsOpen() {
        Context context = getPrivateInstance();
        if (context == null) return false;
        LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        return locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
                || locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
    }

    /**
     * 根据EditText所在坐标和用户点击的坐标相对比，来判断是否隐藏键盘，因为当用户点击EditText时则不能隐藏
     *
     * @param v
     * @param event
     * @return
     */
    public static boolean isShouldHideKeyboard(View v, MotionEvent event) {
        if ((v instanceof EditText)) {
            int[] l = {0, 0};
            v.getLocationOnScreen(l);
            int left = l[0],
                top = l[1],
                bottom = top + v.getHeight(),
                right = left + v.getWidth();
            //点击EditText的时候不做隐藏处理
            return !(event.getRawX() > left) || !(event.getRawX() < right)
                || !(event.getRawY() > top) || !(event.getRawY() < bottom);
        }
        //如果焦点不是EditText则忽略
        return false;
    }

    /**
     * 获取InputMethodManager，隐藏软键盘
     *
     * @param token
     */
    public static void hideKeyboard(IBinder token) {
        Context context = getPrivateInstance();
        if (context == null) return;
        if (token != null) {
            //若token不为空则获取输入法管理器使其隐藏输入法键盘
            InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.hideSoftInputFromWindow(token, InputMethodManager.HIDE_NOT_ALWAYS);
        }
    }

    /**
     * 显示软键盘
     *
     * @param editText
     */
    public static void showKeyboard(@NonNull EditText editText) {
        Context context = getPrivateInstance();
        if (context == null) return;
        if (editText == null) {
            return;
        }
        editText.requestFocus();
        editText.setFocusableInTouchMode(true);
        InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.showSoftInput(editText, InputMethodManager.RESULT_UNCHANGED_SHOWN);
    }

    public static boolean isNull(String s) {
        return s == null || s.trim().isEmpty() || "null".equals(s) || "(null)".equals(s);
    }

    /**
     * 用于进行dip和px转换
     */
    public static int dip2px(float dpValue) {
        final float scale = Resources.getSystem().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    /**
     * 用于进行px和dip转换
     */
    public static int px2dip(float pxValue) {
        final float scale = Resources.getSystem().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }

    /**
     * 启动当前应用设置页面
     */
    public static void startAppSettings() {
        Context context = getPrivateInstance();
        if (context == null) return;
        Intent intent = new Intent(android.provider.Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
        intent.setData(Uri.parse("package:" + context.getPackageName()));
        context.startActivity(intent);
    }

    /**
     * 打开指定App
     *
     * @param packageName
     * @return
     */
    public static boolean openApp(String packageName) {
        Context context = getPrivateInstance();
        if (context == null) return false;
        PackageManager packageManager = context.getPackageManager();
        if (isInstallApp(packageName)) {
            try {
                Intent intent = packageManager.getLaunchIntentForPackage(packageName);
                context.startActivity(intent);
                return true;
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }
        } else {
            return false;
        }
    }

    //检测App是否已安装
    public static boolean isInstallApp(String packageName) {
        Context context = getPrivateInstance();
        if (context == null) return false;
        PackageInfo packageInfo = null;
        try {
            packageInfo = context.getPackageManager().getPackageInfo(packageName, 0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return packageInfo != null;
    }

    /**
     * 获取状态栏的高度
     *
     * @return
     */
    public static int getStatusBarHeight() {
        try {
            Class<?> c = Class.forName("com.android.internal.R$dimen");
            Object obj = c.newInstance();
            Field field = c.getField("status_bar_height");
            int x = Integer.parseInt(field.get(obj).toString());
            return Resources.getSystem().getDimensionPixelSize(x);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return 0;
    }

    /**
     * 获取屏幕宽度
     *
     * @return
     */
    public static int getDisplayWidth() {
        Context context = getPrivateInstance();
        if (context == null) return 0;
        WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display disp = windowManager.getDefaultDisplay();
        Point outP = new Point();
        disp.getSize(outP);
        return outP.x;
    }

    /**
     * 获取屏幕可用部分高度（屏幕高度-状态栏高度-屏幕底栏高度）
     *
     * @return
     */
    public static int getDisplayHeight() {
        Context context = getPrivateInstance();
        if (context == null) return 0;
        WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display disp = windowManager.getDefaultDisplay();
        Point outP = new Point();
        disp.getSize(outP);
        return outP.y;
    }

    /**
     * 获取底栏高度
     *
     * @return
     */
    public static int getNavbarHeight(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            try {
                WindowInsets windowInsets = null;
                windowInsets = activity.getWindow().getDecorView().getRootView().getRootWindowInsets();
                if (windowInsets != null) {
                    return windowInsets.getStableInsetBottom();
                }
            } catch (Exception e) {
            }
        }
        int resourceId = 0;
        int rid = activity.getResources().getIdentifier("config_showNavigationBar", "bool", "android");
        if (rid != 0) {
            resourceId = activity.getResources().getIdentifier("navigation_bar_height", "dimen", "android");
            return activity.getResources().getDimensionPixelSize(resourceId);
        } else {
            return 0;
        }
    }

    /**
     * 获取真实的屏幕高度，注意判断非0
     *
     * @return
     */
    public static int getRootHeight() {
        Context context = getPrivateInstance();
        if (context == null) return 0;
        WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        int displayHeight;
        Display display = windowManager.getDefaultDisplay();
        Point point = new Point();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            display.getRealSize(point);
            displayHeight = point.y;
        } else {
            DisplayMetrics dm = new DisplayMetrics();
            windowManager.getDefaultDisplay().getMetrics(dm);
            displayHeight = dm.heightPixels; //得到高度
        }
        return displayHeight;
    }

    /**
     * 获取WIFI mac地址
     *
     * @return
     */
    public static String getMacAddress() {
        String macAddress;
        StringBuilder buf = new StringBuilder();
        NetworkInterface networkInterface = null;
        try {
            networkInterface = NetworkInterface.getByName("eth1");
            if (networkInterface == null) {
                networkInterface = NetworkInterface.getByName("wlan0");
            }
            if (networkInterface == null) {
                return "02:00:00:00:00:02";
            }
            byte[] addr = networkInterface.getHardwareAddress();
            for (byte b : addr) {
                buf.append(String.format("%02X:", b));
            }
            if (buf.length() > 0) {
                buf.deleteCharAt(buf.length() - 1);
            }
            macAddress = buf.toString();
        } catch (SocketException e) {
            e.printStackTrace();
            return "02:00:00:00:00:02";
        }
        return macAddress;
    }

}
