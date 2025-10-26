package com.example.bluetooth_869_10N_EN.ui.adapter;

import android.graphics.Color;
import android.widget.CheckedTextView;

import androidx.annotation.NonNull;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.viewholder.BaseViewHolder;
import com.example.bluetooth_869_10N_EN.model.DeviceModel;

import java.util.HashSet;
import java.util.Set;

public class DeviceListAdapter extends BaseQuickAdapter<DeviceModel, BaseViewHolder> {

    private Set<String> macs = new HashSet<>();

    public DeviceListAdapter() {
        super(android.R.layout.simple_list_item_single_choice);
    }

    @Override
    protected void convert(@NonNull BaseViewHolder holder, DeviceModel model) {
        String state = model.getType() == 1 ? "已配对" : "未配对";
        String info = model.getDevice().getName() + ", " + model.getDevice().getAddress() + ", " + state;
        CheckedTextView tvInfo = holder.getView(android.R.id.text1);
        tvInfo.setText(info);
        tvInfo.setChecked(model.isSelect());
        tvInfo.setBackgroundColor(tvInfo.isChecked() ? Color.GRAY : Color.WHITE);
    }

    public void addDistData(DeviceModel model) {
        if (macs.add(model.getDevice().getAddress())) {
            addData(model);
        }
    }

    public void clearData() {
        macs.clear();
        getData().clear();
    }

}
