package com.example.bluetooth_869_10N_EN.ui.vm;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.ViewModel;

import com.example.bluetooth_869_10N_EN.business.BtController;

public class HomeViewModel extends ViewModel {

    private final BtController controller;
    private LiveData<String> receiveMsg;

    public HomeViewModel() {
        controller = BtController.getInstance();
        receiveMsg = controller.getReceiveMsg();
    }

    public void sendMessage(byte[] message) {
        controller.sendMessage(message);
    }

    public LiveData<String> getReceiveMsg() {
        return receiveMsg;
    }

    public void disconnectDevice() {
        controller.disconnect();
    }

}