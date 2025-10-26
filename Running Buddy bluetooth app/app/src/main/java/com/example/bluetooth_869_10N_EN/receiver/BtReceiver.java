package com.example.bluetooth_869_10N_EN.receiver;

import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_CONNECTION;
import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_SCAN_FINISH;
import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_SCAN_START;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.text.TextUtils;

import com.example.bluetooth_869_10N_EN.events.DeviceFoundEvent;
import com.example.bluetooth_869_10N_EN.model.DeviceModel;
import com.jeremyliao.liveeventbus.LiveEventBus;

import timber.log.Timber;

public class BtReceiver extends BroadcastReceiver {

    public IntentFilter getFilters() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        return filter;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        // 查找到设备action
        if (BluetoothDevice.ACTION_FOUND.equals(action)) {
            // 得到蓝牙设备
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            if (device != null && !TextUtils.isEmpty(device.getName())) {
                Timber.d("Found device: " + device.getName() + " -> " + device.getAddress());
                DeviceModel model = new DeviceModel(device);
                model.setType(device.getBondState() != BluetoothDevice.BOND_BONDED ? 0 : 1);
                LiveEventBus.get(DeviceFoundEvent.class).post(new DeviceFoundEvent(model));
            }
        } else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
            // 搜索开始action
            LiveEventBus.get(EVENT_SCAN_START).post(true);
        } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
            // 搜索完成action
            LiveEventBus.get(EVENT_SCAN_FINISH).post(true);
        } else if (BluetoothDevice.ACTION_ACL_CONNECTED.equals(action)) {
            LiveEventBus.get(EVENT_CONNECTION).post(true);
        } else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
            LiveEventBus.get(EVENT_CONNECTION).post(false);
        }
    }

}
