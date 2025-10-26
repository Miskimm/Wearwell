package com.example.bluetooth_869_10N_EN.base;

import android.app.Application;
import android.os.Handler;
import android.os.Looper;

public abstract class BaseApp<YourApp extends BaseApp> extends Application {

    private final Handler mainHandler = new Handler(Looper.getMainLooper());

    public abstract void init();

    public YourApp me;
    private static Application instance;

    public static <App extends BaseApp> App getInstance(Class<App> appClass) {
        return (App) instance;
    }

    @Override
    @Deprecated
    public void onCreate() {
        super.onCreate();

        me = (YourApp) this;
        instance = me;

        init();
    }

    public void runOnMain(Runnable runnable) {
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                runnable.run();
            }
        });
    }

    public void runOnMainDelayed(Runnable runnable, long time) {
        mainHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                runOnMain(runnable);
            }
        }, time);
    }

    public void runDelayed(Runnable runnable, long time) {
        mainHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                runnable.run();
            }
        }, time);
    }

    /**
     * 此方法用于主动设置实例化 application 对象以实现一些需要 context 的场合下不需要传参，减少工作量，
     * 在您有实现 App extends BaseApp 的情况下无需调用，
     * 请勿手动调用此方法，此接口仅用于 BaseActivity 以固定逻辑调用。
     *
     * @param context Application 上下文
     * @hide
     */
    public static void setPrivateInstance(Application context) {
        BaseApp.instance = context;
    }

    /**
     * 此方法用于主动获取实例化的 application 对象以实现一些需要 context 的场合下不需要传参，减少工作量，
     * 如需要使用 App 的实例化对象，请勿使用本方法，
     * 请自行实现 getInstance() 方法，或使用 {@link #getInstance(Class)} 来获取。
     *
     * @return 例化的 application 对象
     */
    public static Application getPrivateInstance() {
        if (instance == null) {
            return null;
        }
        return instance;
    }

}