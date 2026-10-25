package com.prammmoelabs.cubbl

import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.BitmapFactory
import android.widget.Button
import android.view.View
import android.widget.FrameLayout
import android.widget.ImageView
import app.cash.paparazzi.DeviceConfig
import app.cash.paparazzi.Paparazzi
import org.junit.Assert.assertEquals
import org.junit.Rule
import org.junit.Test

class GameSceneSnapshotTest {
    @get:Rule val paparazzi = Paparazzi(
        deviceConfig = DeviceConfig.PIXEL_5,
        theme = "Theme.Cubbl"
    )

    private fun scene(background: String, state: GameOverlayState, pressedControl: String? = null, feedback: String? = null) {
        val context = paparazzi.context
        val frame = FrameLayout(context)
        val image = checkNotNull(javaClass.getResourceAsStream("/scenes/$background.png")).use {
            checkNotNull(BitmapFactory.decodeStream(it))
        }
        frame.addView(ImageView(context).apply {
            setImageBitmap(image)
            scaleType = ImageView.ScaleType.FIT_XY
        }, FrameLayout.LayoutParams(-1, -1))
        frame.addView(GameOverlay(context, {}, {}, {}).apply {
            fun dp(value: Int) = (value * resources.displayMetrics.density).toInt()
            // Include a 40dp camera-cutout safe area, as on the capture device.
            setPadding(dp(20), dp(60), dp(20), dp(24))
            render(state)
            if (feedback != null) showFeedback(feedback)
            if (pressedControl != null) {
                val controls = arrayListOf<View>()
                findViewsWithText(controls, pressedControl, View.FIND_VIEWS_WITH_TEXT or View.FIND_VIEWS_WITH_CONTENT_DESCRIPTION)
                check(controls.size == 1)
                controls.single().isPressed = true
            }
        }, FrameLayout.LayoutParams(-1, -1))
        paparazzi.snapshot(frame)
    }

    @Test fun home() = scene("home", GameOverlayState(best = 2400))

    @Test fun credits() = scene("home", GameOverlayState(creditsOpen = true))

    @Test fun homeStartPressed() = scene("home", GameOverlayState(best = 2400), "Start")

    @Test fun pressedButtonHidesEdgeAndReleaseRestoresIt() {
        val context = paparazzi.context
        val overlay = GameOverlay(context, {}, {}, {}).apply { render(GameOverlayState()) }
        val controls = arrayListOf<View>()
        overlay.findViewsWithText(controls, "Start", View.FIND_VIEWS_WITH_TEXT)
        val button = controls.single() as Button
        fun background(): Bitmap = Bitmap.createBitmap(240, 120, Bitmap.Config.ARGB_8888).also {
            button.background.setBounds(0, 0, it.width, it.height)
            button.background.draw(Canvas(it))
        }
        val edge = context.getColor(R.color.ui_terracotta_dark)
        val fill = context.getColor(R.color.ui_terracotta)
        assertEquals(edge, background().getPixel(120, 119))
        button.isPressed = true
        assertEquals(Color.TRANSPARENT, background().getPixel(120, 0))
        assertEquals(fill, background().getPixel(120, 119))
        button.isPressed = false
        assertEquals(edge, background().getPixel(120, 119))
    }

    @Test fun tutorial() = scene("tutorial", GameOverlayState(phase = SWINGING, best = 2400))

    @Test fun gameplay() = scene("tower", GameOverlayState(
        phase = SWINGING, score = 1260, best = 2400, lives = 2, combo = 4, tutorialDone = true
    ))

    @Test fun streakAbovePerfect() = scene("tower", GameOverlayState(
        phase = SWINGING, score = 230, best = 2400, combo = 2, tutorialDone = true
    ), feedback = "Perfect!")

    @Test fun paused() = scene("tower", GameOverlayState(phase = PAUSED, score = 1260, best = 2400))

    @Test fun collapsing() = scene("collapse", GameOverlayState(
        phase = COLLAPSING, score = 1260, best = 2400, lives = 2, tutorialDone = true
    ))

    @Test fun gameOverCollapse() = scene("collapse", GameOverlayState(
        phase = GAME_OVER, score = 1260, best = 2400, lives = 2, reason = 2
    ))

    @Test fun gameOverOutOfLives() = scene("home", GameOverlayState(
        phase = GAME_OVER, score = 0, best = 2400, lives = 0, reason = 1
    ))
}
