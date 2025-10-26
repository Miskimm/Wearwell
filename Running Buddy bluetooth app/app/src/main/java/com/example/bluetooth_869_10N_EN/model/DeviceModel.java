package com.example.bluetooth_869_10N_EN.model;

import android.bluetooth.BluetoothDevice;

public class DeviceModel {

    private BluetoothDevice device;
    private int type; // 0 未配对，1已配对
    private boolean isSelect = false;

    public DeviceModel(BluetoothDevice device) {
        this.device = device;
    }

    public BluetoothDevice getDevice() {
        return device;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public boolean isSelect() {
        return isSelect;
    }

    public void setSelect(boolean select) {
        isSelect = select;
    }

}
