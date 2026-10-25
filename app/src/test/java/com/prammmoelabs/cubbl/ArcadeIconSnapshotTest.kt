package com.prammmoelabs.cubbl

import android.view.ViewGroup
import android.widget.ImageView
import android.widget.LinearLayout
import app.cash.paparazzi.Paparazzi
import com.android.ide.common.rendering.api.SessionParams.RenderingMode
import org.junit.Rule
import org.junit.Test

class ArcadeIconSnapshotTest {
    @get:Rule val paparazzi = Paparazzi(renderingMode = RenderingMode.SHRINK)

    @Test fun arcadeIcons() {
        val context = paparazzi.context
        fun dp(value: Int) = (value * context.resources.displayMetrics.density).toInt()
        val strip = LinearLayout(context).apply {
            orientation = LinearLayout.HORIZONTAL
            layoutParams = ViewGroup.LayoutParams(-2, -2)
            setPadding(dp(12), dp(12), dp(12), dp(12))
            setBackgroundColor(context.getColor(R.color.ui_secondary))
        }
        val icons = listOf(
            ArcadeIcon(ArcadeIcon.Symbol.HEART, context.getColor(R.color.ui_heart)),
            ArcadeIcon(ArcadeIcon.Symbol.HEART, context.getColor(R.color.ui_dark), filled = false),
            ArcadeIcon(ArcadeIcon.Symbol.PAUSE, context.getColor(R.color.ui_dark)),
            ArcadeIcon(ArcadeIcon.Symbol.SETTINGS, context.getColor(R.color.ui_dark))
        )
        icons.forEach { icon ->
            strip.addView(ImageView(context).apply {
                setImageDrawable(icon)
                scaleType = ImageView.ScaleType.FIT_CENTER
                setPadding(dp(12), dp(12), dp(12), dp(12))
            }, LinearLayout.LayoutParams(dp(56), dp(56)))
        }
        paparazzi.snapshot(strip)
    }
}
