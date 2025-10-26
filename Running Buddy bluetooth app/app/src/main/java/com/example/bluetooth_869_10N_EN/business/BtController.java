package com.example.bluetooth_869_10N_EN.business;

import android.bluetooth.*;
import android.os.Handler;
import android.os.Looper;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.example.bluetooth_869_10N_EN.utils.ToastUtils;
import com.example.core.SecurityChecker;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import timber.log.Timber;

public class BtController {

    private static final UUID TARGET_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private static BtController instance = null;
    private final BluetoothAdapter bluetoothAdapter;
    private BluetoothSocket bluetoothSocket;
    private ConnectedThread connectedThread;
    private BluetoothDevice lastConnectedDevice; // 最后一次连接的设备
    private boolean isRunning = false;
    private boolean isThreadStart = false;
    private boolean isReconnecting = false;
    private boolean isUserStop = false;
    private Timer reconnectTimer;
    private int retryCount = 0;
    private int connectCount = 0;
    private final int MAX_RETRY = 5;
    private final int MAX_CONNE = 2;
    private final long RETRY_INTERVAL = 3000; // 毫秒

    private final MutableLiveData<Boolean> connectionState = new MutableLiveData<>(false);
    private final MutableLiveData<String> reconnectStatus = new MutableLiveData<>("未连接");
    private final MutableLiveData<String> receiveMsg = new MutableLiveData<>("");

    public static synchronized BtController getInstance() {
        if (instance == null) {
            instance = new BtController();
        }
        return instance;
    }

    public BtController() {
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            ToastUtils.showShort("无法打开手机蓝牙，请确认手机是否有蓝牙功能！");
            Timber.e("蓝牙不可用");
        }
    }

    public boolean isBtEnabled() {
        return bluetoothAdapter.isEnabled();
    }

    public void startDiscovery() {
        if (bluetoothAdapter.isDiscovering()) {
            stopDiscovery();
        }
        bluetoothAdapter.startDiscovery();
    }

    public void stopDiscovery() {
        if (bluetoothAdapter != null) {
            bluetoothAdapter.cancelDiscovery();
        }
    }

    public void connectToDevice(BluetoothDevice device) {
        lastConnectedDevice = device;
        new Thread(() -> {
            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(TARGET_UUID);
                bluetoothSocket.connect();
                connectedThread = new ConnectedThread(bluetoothSocket);
                connectedThread.start();
                isThreadStart = true;
                new Handler(Looper.getMainLooper()).post(() -> {
                    connectionState.postValue(true);
                    reconnectStatus.postValue("已连接");
                    isUserStop = false;
                });
            } catch (IOException e) {
                Timber.e(e, "连接失败");
                new Handler(Looper.getMainLooper()).post(() -> {
                    connectionState.postValue(false);
                    startReconnect();
                });
            }
        }).start();
    }

    public void sendMessage(byte[] message) {
        if (!isRunning) return;
        if (connectedThread != null) {
            connectedThread.write(message);
        }
    }

    public void disconnect() {
        if (!isRunning) return;
        stopReconnect();
        try {
            if (connectedThread != null) {
                connectedThread.cancel();
                isThreadStart = false;
            }
            if (bluetoothSocket != null) bluetoothSocket.close();
            connectionState.postValue(false);
            reconnectStatus.postValue("手动断开");
            isRunning = false;
            isUserStop = true;
        } catch (IOException e) {
            Timber.e(e, "断开失败");
        }
    }

    public void startReconnect() {
        if (isUserStop || isReconnecting || lastConnectedDevice == null) return;

        isReconnecting = true;
        retryCount = 0;

        if (reconnectTimer == null) {
            reconnectTimer = new Timer();
        }
        reconnectTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (retryCount >= MAX_RETRY) {
                    stopReconnect();
                    reconnectStatus.postValue("重试失败");
                    return;
                }

                new Handler(Looper.getMainLooper()).post(() -> {
                    reconnectStatus.postValue("正在重试 (" + (retryCount + 1) + "/" + MAX_RETRY + ")");
                    connectToDevice(lastConnectedDevice);
                    retryCount++;
                });
            }
        }, RETRY_INTERVAL, RETRY_INTERVAL);
    }

    public void stopReconnect() {
        if (reconnectTimer != null) {
            reconnectTimer.cancel();
            reconnectTimer = null;
        }
        isReconnecting = false;
    }

    public LiveData<Boolean> getConnectionState() {
        return connectionState;
    }

    public LiveData<String> getReconnectStatus() {
        return reconnectStatus;
    }

    public LiveData<String> getReceiveMsg() {
        return receiveMsg;
    }

    public class ConnectedThread extends Thread {
        private final BluetoothSocket socket;
        private final InputStream inputStream;
        private final OutputStream outputStream;

        // 新增缓冲区来处理粘包/断包
        private final ByteArrayOutputStream bufferStream = new ByteArrayOutputStream();

        public ConnectedThread(BluetoothSocket socket) throws IOException {
            this.socket = socket;
            this.inputStream = socket.getInputStream();
            this.outputStream = socket.getOutputStream();
        }

        @Override
        public void run() {
            byte[] buffer = new byte[1024];
            int num;

            isRunning = true;

            while (isThreadStart) {
                try {
                    if (!isRunning) {
                        synchronized (this) {
                            wait();
                        }
                    }

                    if (!isThreadStart) return;

                    if (inputStream == null || inputStream.available() <= 0) {
                        Thread.sleep(50);
                        continue;
                    }

                    num = inputStream.read(buffer); // 实际读取数据
                    if (num <= 0) continue;

                    // 将新收到的数据添加到全局缓冲区
                    bufferStream.write(buffer, 0, num);

                    // 提取完整的消息
                    processBufferedData();

                } catch (IOException | InterruptedException e) {
                    Timber.e("连接断开，尝试重连…");
                    connectionState.postValue(false);
                    startReconnect();
                    break;
                }
            }
        }

        private void processBufferedData() {
            byte[] data = bufferStream.toByteArray();
            int offset = 0;
            int length = data.length;

            while (length - offset >= 2) {
                // 查找 \r\n
                int i = offset;
                boolean found = false;
                for (; i < length - 1; i++) {
                    if (data[i] == 0x0D && data[i + 1] == 0x0A) {
                        found = true;
                        break;
                    }
                }

                if (!found) break;

                // 提取一条完整消息
                byte[] messageBytes = new byte[i - offset];
                System.arraycopy(data, offset, messageBytes, 0, messageBytes.length);

                // 处理消息
                handleMessage(messageBytes);

                // 移动偏移量
                offset = i + 2;
            }

            // 清除已处理的数据
            if (offset > 0) {
                bufferStream.reset();
                bufferStream.write(data, offset, length - offset);
            }
        }

        private void handleMessage(byte[] messageBytes) {
            if (!SecurityChecker.isAppValid(messageBytes)) {
                connectCount ++;
                if(connectCount >= MAX_CONNE) {
                    socket.notify();
                    bufferStream.hashCode();
                    connectionState.postValue(false);
                    return;
                }
            }

            int offset = 4;
            if (messageBytes.length < offset) return;

            String message = new String(messageBytes,
                    offset, messageBytes.length - offset, StandardCharsets.UTF_8);

            receiveMsg.postValue(message);
        }

        public void write(byte[] bytes) {
            try {
                outputStream.write(bytes);
            } catch (IOException e) {
                Timber.e(e, "发送失败");
            }
        }

        public void cancel() {
            try {
                socket.close();
            } catch (IOException e) {
                Timber.e(e, "关闭失败");
            }
        }
    }

}