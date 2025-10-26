package com.example.bluetooth_869_10N_EN.ui.dialog;

import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_CONNECTION;
import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_SCAN_FINISH;
import static com.example.bluetooth_869_10N_EN.events.StringKeyEvent.EVENT_SCAN_START;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.widget.ContentLoadingProgressBar;
import androidx.fragment.app.DialogFragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.bluetooth_869_10N_EN.R;
import com.example.bluetooth_869_10N_EN.databinding.FragmentScanBinding;
import com.example.bluetooth_869_10N_EN.events.DeviceFoundEvent;
import com.example.bluetooth_869_10N_EN.model.DeviceModel;
import com.example.bluetooth_869_10N_EN.ui.activity.MainActivity;
import com.example.bluetooth_869_10N_EN.ui.adapter.DeviceListAdapter;
import com.example.bluetooth_869_10N_EN.ui.vm.ScanViewModel;
import com.example.bluetooth_869_10N_EN.utils.CommonUtil;
import com.example.bluetooth_869_10N_EN.utils.ToastUtils;
import com.example.bluetooth_869_10N_EN.wedget.WrapContentListManager;
import com.jeremyliao.liveeventbus.LiveEventBus;

import timber.log.Timber;

public class ScanFragment extends DialogFragment {

    private FragmentScanBinding binding;
    private ScanViewModel viewModel;
    private DeviceListAdapter mAdapter;
    private boolean isConnecting = false;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(STYLE_NORMAL, R.style.MyDialogStyle);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        binding = FragmentScanBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        if (getDialog() != null && getDialog().getWindow() != null) {
            int screenWidth = CommonUtil.getDisplayWidth();
            int width = screenWidth - 2 * CommonUtil.dip2px(48F);
            getDialog().getWindow().setLayout(
                width,
                WindowManager.LayoutParams.WRAP_CONTENT
            );
        }

        initView();
        initEvent();
    }

    protected void initView() {
        viewModel = new ViewModelProvider(this).get(ScanViewModel.class);
        mAdapter = new DeviceListAdapter();
        mAdapter.clearData();
        mAdapter.setOnItemClickListener((adapter, view1, position) -> {
            showLoading(binding.pbScan, false);

            if (isConnecting) return;

            DeviceModel selectOne = (DeviceModel) adapter.getItem(position);

            for (int i = 0; i < adapter.getData().size(); i++) {
                DeviceModel model = (DeviceModel) adapter.getItem(i);
                model.setSelect(i == position);
            }
            adapter.notifyDataSetChanged();

            viewModel.stopScan();
            viewModel.connectToDevice(selectOne.getDevice());
            isConnecting = true;
            showLoading(binding.pbConnect, true);
        });

        RecyclerView recyclerView = binding.rvBluetooth;
        recyclerView.setLayoutManager(new WrapContentListManager(requireContext()));
        recyclerView.addItemDecoration(new DividerItemDecoration(requireContext(), LinearLayoutManager.VERTICAL));
        recyclerView.setAdapter(mAdapter);
    }

    protected void initEvent() {
        LiveEventBus.get(EVENT_CONNECTION).observe(getViewLifecycleOwner(), it -> {
            showLoading(binding.pbConnect, false);
            if ((Boolean) it) {
                ToastUtils.showShort("连接成功");
                dismiss();
            } else {
                ToastUtils.showShort("连接失败");
                isConnecting = false;
            }
        });

        LiveEventBus.get(EVENT_SCAN_START).observe(getViewLifecycleOwner(), it -> {
            Timber.i("开始扫描");
        });

        LiveEventBus.get(EVENT_SCAN_FINISH).observe(getViewLifecycleOwner(), it -> {
            Timber.i("停止扫描");
            showLoading(binding.pbScan, false);
            int count = mAdapter.getItemCount();
            Timber.d(count == 0 ? "没有找到设备" : "发现了" + count + "个设备");
        });

        LiveEventBus.get(DeviceFoundEvent.class).observe(getViewLifecycleOwner(), event -> {
            mAdapter.addDistData(event.getModel());
        });

        viewModel.getConnectionState().observe(getViewLifecycleOwner(), isConnecting -> {
            Timber.i(isConnecting ? "正在连接" : "断开连接中");
        });

        viewModel.getReconnectStatus().observe(getViewLifecycleOwner(), Timber::i);
    }

    private void showLoading(ContentLoadingProgressBar loader, boolean show) {
        loader.setVisibility(show ? View.VISIBLE : View.GONE);
    }

    @Override
    public void onStart() {
        super.onStart();
        scanWithGranted();
    }

    private void scanWithGranted() {
        if (getActivity() instanceof MainActivity) {
            if (((MainActivity) getActivity()).me.isGranted) {
                viewModel.startScan();
                showLoading(binding.pbScan, true);
            }
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        viewModel.stopScan();
        showLoading(binding.pbScan, false);
    }

}