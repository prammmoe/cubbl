package com.example.cubbl

import android.graphics.Canvas
import android.graphics.ColorFilter
import android.graphics.Paint
import android.graphics.Path
import android.graphics.PixelFormat
import android.graphics.drawable.Drawable
import androidx.core.graphics.withSave

internal class ArcadeIcon(private val symbol: Symbol, color: Int, private val filled: Boolean = true) : Drawable() {
    enum class Symbol { HEART, PAUSE, SETTINGS }
    private val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        this.color = color
        strokeWidth = 2.5f
        strokeCap = Paint.Cap.ROUND
        strokeJoin = Paint.Join.ROUND
    }
    private val heart = Path().apply {
        moveTo(12f, 21f)
        cubicTo(9f, 18f, 2f, 13f, 2f, 7.5f)
        cubicTo(2f, 1.5f, 9f, 1f, 12f, 5.3f)
        cubicTo(15f, 1f, 22f, 1.5f, 22f, 7.5f)
        cubicTo(22f, 13f, 15f, 18f, 12f, 21f)
        close()
    }
    override fun draw(canvas: Canvas) {
        canvas.withSave {
            canvas.translate(bounds.left.toFloat(), bounds.top.toFloat())
            canvas.scale(bounds.width() / 24f, bounds.height() / 24f)
            paint.style = Paint.Style.FILL
            when (symbol) {
                Symbol.HEART -> {
                    paint.style = if (filled) Paint.Style.FILL else Paint.Style.STROKE
                    paint.strokeWidth = 1.8f
                    canvas.drawPath(heart, paint)
                }
                Symbol.PAUSE -> {
                    canvas.drawRoundRect(5f, 3f, 10f, 21f, 1f, 1f, paint)
                    canvas.drawRoundRect(14f, 3f, 19f, 21f, 1f, 1f, paint)
                }
                Symbol.SETTINGS -> {
                    paint.strokeWidth = 2.5f
                    for (row in 0..2) {
                        val y = 5f + row * 7f
                        canvas.drawLine(3f, y, 21f, y, paint)
                        canvas.drawCircle(if (row == 1) 15f else 8f, y, 3.2f, paint)
                    }
                }
            }
        }
    }
    override fun setAlpha(alpha: Int) { paint.alpha = alpha; invalidateSelf() }
    override fun setColorFilter(colorFilter: ColorFilter?) { paint.colorFilter = colorFilter; invalidateSelf() }
    @Suppress("DEPRECATION")
    override fun getOpacity() = PixelFormat.TRANSLUCENT
    override fun getIntrinsicWidth() = 24
    override fun getIntrinsicHeight() = 24
}
