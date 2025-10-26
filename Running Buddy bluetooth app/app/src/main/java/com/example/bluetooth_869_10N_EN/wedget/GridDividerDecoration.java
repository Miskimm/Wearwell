package com.example.bluetooth_869_10N_EN.wedget;

import android.graphics.Rect;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import timber.log.Timber;

public class GridDividerDecoration extends RecyclerView.ItemDecoration {

    private final int mSpanCount;   // 列数
    private final int mRowSpacing;  // 行间距
    private final int mColumnSpacing; // 列间距

    public GridDividerDecoration(int spanCount, int rowSpacing, int columnSpacing) {
        this.mSpanCount = spanCount;
        this.mRowSpacing = rowSpacing;
        this.mColumnSpacing = columnSpacing;
    }

    @Override
    public void getItemOffsets(@NonNull Rect outRect, @NonNull View view, @NonNull RecyclerView parent, @NonNull RecyclerView.State state) {
        super.getItemOffsets(outRect, view, parent, state);

        int position = parent.getChildAdapterPosition(view); // 获取 view 在 adapter 中的位置
        int column = position % mSpanCount; // view 所在的列

        outRect.left = column * mColumnSpacing / mSpanCount;
        outRect.right = mColumnSpacing - (column + 1) * mColumnSpacing / mSpanCount;

        Timber.e("position:" + position
            + "    columnIndex: " + column
            + "    left,right ->" + outRect.left + "," + outRect.right);

        // 如果 position >= spanCount，说明不是在第一行，则 top 设置为行间距
        if (position >= mSpanCount) {
            outRect.top = mRowSpacing; // item top
        }
    }
}