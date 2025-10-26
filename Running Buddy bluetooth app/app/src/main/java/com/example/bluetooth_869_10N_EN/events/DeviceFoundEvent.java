package com.example.bluetooth_869_10N_EN.events;

import com.example.bluetooth_869_10N_EN.model.DeviceModel;
import com.jeremyliao.liveeventbus.core.LiveEvent;

public class DeviceFoundEvent implements LiveEvent {

    private DeviceModel model;

    public DeviceFoundEvent(DeviceModel model) {
        this.model = model;
    }

    public DeviceModel getModel() {
        return model;
    }

}
