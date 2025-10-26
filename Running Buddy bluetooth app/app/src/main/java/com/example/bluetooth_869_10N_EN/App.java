package com.example.bluetooth_869_10N_EN;

import com.example.bluetooth_869_10N_EN.base.BaseApp;
import com.example.bluetooth_869_10N_EN.utils.ReleaseTree;
import com.jeremyliao.liveeventbus.LiveEventBus;
import com.kongzue.dialogx.DialogX;

import timber.log.Timber;

public class App extends BaseApp<App> {

    @Override
    public void init() {
        DialogX.init(this);

        boolean needLog = BuildConfig.DEBUG;

        if (needLog) {
            Timber.plant(new Timber.DebugTree());
        } else {
            Timber.plant(new ReleaseTree());
        }


        LiveEventBus
            .config()
            .autoClear(true)
            .enableLogger(needLog)
            .lifecycleObserverAlwaysActive(true);
    }

}
