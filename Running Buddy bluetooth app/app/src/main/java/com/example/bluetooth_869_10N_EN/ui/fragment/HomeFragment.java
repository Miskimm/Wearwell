package com.example.bluetooth_869_10N_EN.ui.fragment;

import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProvider;

import com.example.bluetooth_869_10N_EN.base.BaseFragment;
import com.example.bluetooth_869_10N_EN.base.Constant;
import com.example.bluetooth_869_10N_EN.databinding.FragmentHomeBinding;
import com.example.bluetooth_869_10N_EN.ui.vm.HomeViewModel;
import com.example.bluetooth_869_10N_EN.utils.AppManager;
import com.example.bluetooth_869_10N_EN.utils.ToastUtils;
import com.jeremyliao.liveeventbus.LiveEventBus;
import com.kongzue.dialogx.dialogs.MessageDialog;

import androidx.navigation.Navigation;

import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_CONNECTION;

import java.text.SimpleDateFormat;
import java.util.Date;

public class HomeFragment extends BaseFragment<FragmentHomeBinding> {

    private HomeViewModel viewModel;
    private String currentTime1;
    private String currentTime2;
    private String currentTime3;

    @Override
    protected void initView() {
        viewModel = new ViewModelProvider(this).get(HomeViewModel.class);
        new TimeThread().start(); //启动新的线程
    }
    class TimeThread extends Thread {
        @Override
        public void run() {
            do {
                try {
                    Thread.sleep(1000);
                    Message msg = new Message();
                    msg.what = 1;  //消息(一个整型值)
                    mHandler.sendMessage(msg);// 每隔1秒发送一个msg给mHandler
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } while (true);
        }
    }
    //在主线程里面处理消息并更新UI界面
    private Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 1:
                    SimpleDateFormat sdf1=new SimpleDateFormat("yyyy/MM/dd");
                    currentTime1=sdf1.format(new Date());
                    binding.time1.setText(currentTime1); //更新时间

                    SimpleDateFormat sdf=new SimpleDateFormat("HH:mm:ss");
                    currentTime2=sdf.format(new Date());
                    binding.time2.setText(currentTime2); //更新时间

                    SimpleDateFormat sdf3=new SimpleDateFormat("yyyy/MM/dd");
                    currentTime3=sdf3.format(new Date());

                    break;
                default:
                    break;
            }
        }
    };

    @Override
    protected void initEvent() {
        binding.btnSend.setOnClickListener(v -> {

            String message = "&"+ "hear_min:"+(binding.heartRateMin.getText().toString())+","+
                             "hear_max:"+(binding.heartRateMax.getText().toString())+","+
                             "spo_min:"+(binding.spo2Min.getText().toString())+","+
                             "temp_min:"+(binding.tempMin.getText().toString())+","+
                             "temp_max:"+(binding.tempMax.getText().toString())+","+
                             "set_step:"+(binding.setStep.getText().toString())+ "\r\n";

            viewModel.sendMessage(message.getBytes());
        });

        binding.btnSwitchColor.setOnClickListener(v -> {
            String message = "&"+"SwitchColor"+ "\r\n";
            viewModel.sendMessage(message.getBytes());
        });

        binding.btnSwitchDisplay.setOnClickListener(v -> {
            String message = "&"+"SwitchDisplay"+ "\r\n";
            viewModel.sendMessage(message.getBytes());
        });

        binding.btnStart.setOnClickListener(v -> {
            String message = "&"+"start"+ "\r\n";
            viewModel.sendMessage(message.getBytes());
        });

        binding.btnCleanStep.setOnClickListener(v -> {
            String message = "&"+"CleanStep"+ "\r\n";
            viewModel.sendMessage(message.getBytes());
        });

        binding.btnCleanTime.setOnClickListener(v -> {
            String message = "&"+"CleanTime"+ "\r\n";
            viewModel.sendMessage(message.getBytes());
        });

        binding.btnSetTime.setOnClickListener(v -> {
            String message = "&"+"year:"+currentTime3+",hour:"+currentTime2+"\r\n";

            viewModel.sendMessage(message.getBytes());

        });

        binding.btnDisconnect.setOnClickListener(v -> {
            viewModel.disconnectDevice();
        });

        binding.btnScan.setOnClickListener(v -> {
            Navigation.findNavController(requireView()).navigate(
                    HomeFragmentDirections.actionHomeFragmentToScanFragment()
            );
        });

        binding.btnExit.setOnClickListener(v -> {
            viewModel.disconnectDevice();
            AppManager.getInstance().exit(requireContext());
        });

        viewModel.getReceiveMsg().observe(getViewLifecycleOwner(), msg -> {

            if (Constant.NEED_UPDATE.equals(msg)) {
                showUpdateDialog();
            }

            int intIndex1 = msg.indexOf("hear:");
            if(intIndex1!=-1) {
                binding.TextViewHeartRate.setText((msg.substring(intIndex1+5,msg.indexOf("#",intIndex1))));   //显示数据
                if(msg.contains("hear_warn")) {
                    binding.TextViewHeartRate.setTextColor(0xffff0000);
                }
                else {
                    binding.TextViewHeartRate.setTextColor(0xff00BFA5);
                }
            }
            int intIndex2 = msg.indexOf("spo2:");
            if(intIndex2!=-1) {
                binding.TextViewSpo2.setText((msg.substring(intIndex2+5,msg.indexOf("#",intIndex2))));   //显示数据
                if(msg.contains("spo2_warn")) {
                    binding.TextViewSpo2.setTextColor(0xffff0000);
                    binding.textViewPercentage.setTextColor(0xffff0000);
                }
                else {
                    binding.TextViewSpo2.setTextColor(0xff00BFA5);
                    binding.textViewPercentage.setTextColor(0xff00BFA5);
                }
            }
            int intIndex3 = msg.indexOf("temp:");
            if(intIndex3!=-1) {
                binding.TextViewTemp.setText((msg.substring(intIndex3+5,msg.indexOf("#",intIndex3))));   //显示数据
                if(msg.contains("temp_warn")) {
                    binding.TextViewTemp.setTextColor(0xffff0000);
                    binding.textViewCentigrade.setTextColor(0xffff0000);
                }
                else {
                    binding.TextViewTemp.setTextColor(0xff00BFA5);
                    binding.textViewCentigrade.setTextColor(0xff00BFA5);
                }
            }
            int intIndex4 = msg.indexOf("step:");
            if(intIndex4!=-1) {
                binding.TextViewStep.setText((msg.substring(intIndex4+5,msg.indexOf("#",intIndex4))));   //显示数据
                if(msg.contains("step_warn")) {
                    binding.TextViewStep.setTextColor(0xffff0000);
                }
                else {
                    binding.TextViewStep.setTextColor(0xff00BFA5);
                }
            }
            int intIndex7 = msg.indexOf("time:");
            if(intIndex7!=-1) {
                binding.TextViewExerciseTime.setText((msg.substring(intIndex7+5,msg.indexOf("#",intIndex7))));   //显示数据
            }
            int intIndex8 = msg.indexOf("lon:");
            if(intIndex8!=-1) {
                binding.TextViewLon.setText((msg.substring(intIndex8+4,msg.indexOf("#",intIndex8))));   //显示数据
            }
            int intIndex9 = msg.indexOf("lat:");
            if(intIndex9!=-1) {
                binding.TextViewLat.setText((msg.substring(intIndex9+4,msg.indexOf("#",intIndex9))));   //显示数据
            }
        });

        LiveEventBus.get(EVENT_CONNECTION).observe(this, it -> {
            if (!(Boolean) it) {
                ToastUtils.showShort("连接已断开");
            }
        });
    }

    private void showUpdateDialog() {
        MessageDialog.build()
                .setTitle("提示")
                .setMessage(Constant.NEED_UPDATE)
                .setOkButton("好的", (dialog, v) -> {
                    viewModel.disconnectDevice();
                    AppManager.getInstance().exit(requireContext());
                    return false;
                })
                .setCancelable(false)
                .show();
    }

    @Override
    public FragmentHomeBinding getViewBinding(@NonNull LayoutInflater inflater, @Nullable ViewGroup container) {
        return FragmentHomeBinding.inflate(inflater, container, false);
    }

}