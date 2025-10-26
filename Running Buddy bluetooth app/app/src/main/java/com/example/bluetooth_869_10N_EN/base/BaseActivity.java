package com.example.bluetooth_869_10N_EN.base;

import static com.example.bluetooth_869_10N_EN.base.Constant.NEED_BLUETOOTH;
import static com.example.bluetooth_869_10N_EN.base.Constant.NEED_LOCATION;
import static com.example.bluetooth_869_10N_EN.utils.CommonUtil.checkGPSIsOpen;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;

import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.activity.result.ActivityResultLauncher;
import androidx.viewbinding.ViewBinding;

import com.example.bluetooth_869_10N_EN.business.BtController;
import com.example.bluetooth_869_10N_EN.utils.AppManager;
import com.kongzue.dialogx.dialogs.MessageDialog;

import timber.log.Timber;

public abstract class BaseActivity<VB extends ViewBinding> extends AppCompatActivity {

    public boolean isGranted;
    private ActivityResultLauncher<Intent> openDetailLauncher;
    private ActivityResultLauncher<String[]> requestBtLauncher;
    private ActivityResultLauncher<String[]> requestLocLauncher;
    private ActivityResultLauncher<Intent> openBTLauncher;
    private ActivityResultLauncher<Intent> openGpsLauncher;

    protected VB binding;
    public BaseActivity<VB> me = this;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (BaseApp.getPrivateInstance() == null) {
            BaseApp.setPrivateInstance(getApplication());
        }
        AppManager.getInstance().pushActivity(me);
        registerLaunchers();

        binding = getViewBinding();
        setContentView(binding.getRoot());
        initViews();
        initEvents();
    }

    public abstract void initViews();

    public abstract void initEvents();

    public abstract VB getViewBinding();

    @Override
    protected void onResume() {
        super.onResume();
        AppManager.setActiveActivity(this);
    }

    private void registerLaunchers() {
        openDetailLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && Build.VERSION.SDK_INT < Build.VERSION_CODES.S) {
                    if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                        == PackageManager.PERMISSION_GRANTED) {
                        onPermissionGranted();
                    } else {
                        Timber.d("定位权限还是没开");
                        finish();
                    }
                } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN)
                        == PackageManager.PERMISSION_GRANTED) {
                        onPermissionGranted();
                    } else {
                        Timber.d("蓝牙扫描权限还是没开");
                        finish();
                    }
                }
            }
        );

        requestBtLauncher = registerForActivityResult(
            new ActivityResultContracts.RequestMultiplePermissions(),
            result -> {
                boolean allGranted = true;
                for (boolean granted : result.values()) {
                    if (!granted) {
                        allGranted = false;
                        break;
                    }
                }
                if (allGranted) {
                    onPermissionGranted();
                } else {
                    showNeedPermissionDialog(NEED_BLUETOOTH);
                }
            }
        );

        requestLocLauncher = registerForActivityResult(
            new ActivityResultContracts.RequestMultiplePermissions(),
            result -> {
                boolean allGranted = true;
                for (boolean granted : result.values()) {
                    if (!granted) {
                        allGranted = false;
                        break;
                    }
                }
                if (allGranted) {
                    onPermissionGranted();
                } else {
                    showNeedPermissionDialog(NEED_LOCATION);
                }
            }
        );

        openBTLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (BtController.getInstance().isBtEnabled()) {
                    onPermissionGranted();
                } else {
                    Timber.d("蓝牙还是没开");
                    finish();
                }
            }
        );

        openGpsLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (checkGPSIsOpen()) {
                    onPermissionGranted();
                } else {
                    Timber.d("GPS还是没开");
                    finish();
                }
            }
        );
    }

    protected void handlePermission() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            onPermissionGranted();
        } else if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                == PackageManager.PERMISSION_GRANTED) {
                onPermissionGranted();
            } else if (shouldShowRequestPermissionRationale(Manifest.permission.ACCESS_COARSE_LOCATION)) {
                showNeedPermissionDialog(NEED_LOCATION);
            } else {
                requestLocLauncher.launch(new String[]{
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION
                });
            }
        } else {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN)
                == PackageManager.PERMISSION_GRANTED) {
                onPermissionGranted();
            } else if (shouldShowRequestPermissionRationale(Manifest.permission.BLUETOOTH_SCAN)) {
                showNeedPermissionDialog(NEED_BLUETOOTH);
            } else {
                requestBtLauncher.launch(new String[]{
                    Manifest.permission.BLUETOOTH_SCAN,
                    Manifest.permission.BLUETOOTH_CONNECT,
                    Manifest.permission.ACCESS_FINE_LOCATION
                });
            }
        }
    }

    private void showTurnOnLocationDialog() {
        MessageDialog.build()
            .setTitle("提示")
            .setMessage("请打开定位开关")
            .setCancelButton("取消", (dialog, v) -> {
                finish();
                return false;
            })
            .setOkButton("好的", (dialog, v) -> {
                openGpsLauncher.launch(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS));
                return false;
            })
            .setCancelable(false)
            .show();
    }

    private void showNeedPermissionDialog(String message) {
        new MessageDialog("提示", String.format("本应用需要“%s”来扫描蓝牙设备，请授权！", message))
            .setOkButton("设置", (dialog, v) -> {
                openDetailLauncher.launch(new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
                    .setData(Uri.parse("package:" + getPackageName())));
                return false;
            })
            .setCancelButton("取消", (dialog, v) -> {
                finish();
                return false;
            })
            .setCancelable(false)
            .show();
    }

    private void onPermissionGranted() {
        if (!BtController.getInstance().isBtEnabled()) {
            openBTLauncher.launch(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE));
            return;
        }
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S) {
            if (!checkGPSIsOpen()) {
                showTurnOnLocationDialog();
                return;
            }
        }
        isGranted = true;
    }

    @Override
    public void finish() {
        AppManager.getInstance().killActivity(me);
    }

    public void finishActivity() {
        super.finish();
    }

    @Override
    protected void onDestroy() {
        AppManager.getInstance().deleteActivity(me);
        super.onDestroy();
    }

}