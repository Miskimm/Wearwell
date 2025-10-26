package com.example.bluetooth_869_10N_EN.base;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.viewbinding.ViewBinding;

import com.example.bluetooth_869_10N_EN.App;
import com.kongzue.dialogx.dialogs.WaitDialog;

public abstract class BaseFragment<VB extends ViewBinding> extends Fragment {

    protected VB binding;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        binding = getViewBinding(inflater, container);
        initView();
        initEvent();
        return binding.getRoot();
    }

    public void showLoading(boolean autoDismiss) {
        WaitDialog.show("请稍后…");
        if (autoDismiss) {
            BaseApp.getInstance(App.class).runOnMainDelayed(this::hideLoading, 3000);
        }
    }

    public void showLoading() {
        showLoading(true);
    }

    public void hideLoading() {
        WaitDialog.dismiss();
    }

    protected abstract void initView();

    protected abstract void initEvent();

    public abstract VB getViewBinding(@NonNull LayoutInflater inflater, @Nullable ViewGroup container);

}