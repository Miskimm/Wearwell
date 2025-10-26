package com.example.bluetooth_869_10N_EN.ui.vm;

import android.bluetooth.BluetoothDevice;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.ViewModel;

import com.example.bluetooth_869_10N_EN.business.BtController;

public class ScanViewModel extends ViewModel {

    private BtController controller;
    private LiveData<Boolean> connectionState;
    private LiveData<String> reconnectStatus;

    public ScanViewModel() {
        controller = BtController.getInstance();
        connectionState = controller.getConnectionState();
        reconnectStatus = controller.getReconnectStatus();
    }

    public void startScan() {
        controller.startDiscovery();
    }

    public void stopScan() {
        controller.stopDiscovery();
    }

    public void connectToDevice(BluetoothDevice device) {
        controller.connectToDevice(device);
    }

    public LiveData<Boolean> getConnectionState() {
        return connectionState;
    }

    public LiveData<String> getReconnectStatus() {
        return reconnectStatus;
    }

}