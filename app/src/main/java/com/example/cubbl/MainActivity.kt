package com.example.cubbl

import android.view.ContextThemeWrapper
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.VibrationEffect
import android.os.Vibrator
import android.os.VibratorManager
import android.view.WindowInsets
import android.view.WindowInsetsController
import androidx.appcompat.app.AlertDialog
import android.widget.FrameLayout
import android.widget.ImageView
import androidx.activity.OnBackPressedCallback
import com.google.androidgamesdk.GameActivity

class MainActivity : GameActivity() {
    companion object {
        init { System.loadLibrary("cubbl") }
    }
    private external fun nativeCommand(action: Int)
    private external fun nativeSnapshot(): LongArray
    private external fun nativeHasRenderedFrame(): Boolean
    private var launchCity: ImageView? = null
    private lateinit var root: GameOverlay
    private lateinit var preferences: PlayerPreferences
    private lateinit var audio: GameAudio
    private val handler = Handler(Looper.getMainLooper())
    private var phase = -1
    private var score = 0L
    private var reason = 0
    private var lives = 3
    private var combo = 0
    private var lastSequence = 0L
    private var failed = false
    private var releasePending = false
    private var settingsDialog: AlertDialog? = null
    private var restoreSettings = false
    private var creditsOpen = false
    private val backCallback = object : OnBackPressedCallback(false) {
        override fun handleOnBackPressed() {
            if (creditsOpen) {
                showCredits(false)
            } else if (phase == PAUSED || phase == GAME_OVER) nativeCommand(MENU)
            else nativeCommand(PAUSE)
        }
    }
    private var currentSnapshot: LongArray? = null
    private val hideFeedback = Runnable { root.clearFeedback() }
    private val polling = object : Runnable {
        override fun run() {
            val snapshot = nativeSnapshot()
            if (currentSnapshot?.contentEquals(snapshot) != true) {
                currentSnapshot = snapshot
                update(snapshot)
            }
            if (launchCity != null && (nativeHasRenderedFrame() || failed)) {
                (launchCity?.parent as? FrameLayout)?.removeView(launchCity)
                launchCity = null
            }
            handler.postDelayed(this, 50)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        preferences = PlayerPreferences(getSharedPreferences("cubbl", MODE_PRIVATE))
        audio = GameAudio(this)
        lastSequence = nativeSnapshot().getOrElse(6) { 0L }
        creditsOpen = savedInstanceState?.getBoolean("credits_open") == true
        root = GameOverlay(this, ::onUiAction, ::showSettings, ::finish, onCredits = ::showCredits).apply {
            setOnApplyWindowInsetsListener { view, insets ->
                val safe = insets.getInsets(WindowInsets.Type.systemBars() or WindowInsets.Type.displayCutout())
                view.setPadding(dp(20) + safe.left, dp(20) + safe.top, dp(20) + safe.right, dp(24) + safe.bottom)
                insets
            }
        }
        addContentView(root, FrameLayout.LayoutParams(-1, -1))
        launchCity = ImageView(this).apply {
            tag = "launch_city"
            setImageResource(R.drawable.launch_city)
            scaleType = ImageView.ScaleType.CENTER_CROP
            isClickable = true
            importantForAccessibility = android.view.View.IMPORTANT_FOR_ACCESSIBILITY_NO
        }.also { addContentView(it, FrameLayout.LayoutParams(-1, -1)) }
        if (Build.VERSION.SDK_INT >= 31) {
            splashScreen.setOnExitAnimationListener { it.remove() }
        }
        onBackPressedDispatcher.addCallback(this, backCallback)
        renderUi()
        root.requestApplyInsets()
        hideSystemUi()
        restoreSettings = savedInstanceState?.getBoolean("settings_open") == true
    }

    override fun onSaveInstanceState(outState: Bundle) {
        outState.putBoolean("settings_open", settingsDialog?.isShowing == true || restoreSettings)
        outState.putBoolean("credits_open", creditsOpen)
        super.onSaveInstanceState(outState)
    }

    override fun onResume() {
        super.onResume()
        if (::root.isInitialized) {
            // A recreated window can retain focus without emitting a new focus-gain callback.
            root.post {
                if (!isDestroyed && lifecycle.currentState.isAtLeast(androidx.lifecycle.Lifecycle.State.RESUMED)) {
                    onWindowFocusChangedNative(gameActivityNativeHandle, root.hasWindowFocus())
                }
            }
            handler.removeCallbacks(polling)
            handler.post(polling)
        }
    }
    override fun onPause() {
        handler.removeCallbacks(polling)
        handler.removeCallbacks(hideFeedback)
        if (::audio.isInitialized) audio.stop()
        super.onPause()
    }
    override fun onDestroy() {
        settingsDialog?.dismiss()
        handler.removeCallbacksAndMessages(null)
        audio.release()
        super.onDestroy()
    }
    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
            hideSystemUi()
            if (restoreSettings) {
                restoreSettings = false
                showSettings()
            }
        } else if (::audio.isInitialized) audio.stop()
    }
    private fun hideSystemUi() {
        window.insetsController?.apply {
            hide(WindowInsets.Type.systemBars())
            systemBarsBehavior = WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
    }
    private fun update(s: LongArray) {
        if (s.size != 9) return
        val nextPhase = s[0].toInt()
        reason = s[8].toInt()
        if (nextPhase != SWINGING) releasePending = false
        val previousCombo = combo
        score = s[1]; combo = s[2].toInt(); lives = s[3].toInt()
        preferences.record(score)
        if (s[4] > 0) preferences.completeTutorial()
        val errorChanged = failed != (s[7] == 1L)
        failed = s[7] == 1L
        if (phase != nextPhase || errorChanged) {
            phase = nextPhase
            renderUi()
            if (phase == PAUSED) audio.stop()
        }
        root.update(overlayState())
        if (s[6] != lastSequence) {
            lastSequence = s[6]
            when (if (phase == PAUSED || phase == MENU_PHASE) -1 else s[5].toInt()) {
                0 -> feedback(getString(R.string.feedback_perfect), GameAudio.perfectSound(previousCombo, combo))
                1 -> feedback(getString(R.string.feedback_good), 1)
                2 -> feedback(getString(R.string.feedback_bad), 1)
                3 -> feedback(getString(R.string.feedback_miss), GameAudio.MISS)
                5 -> feedback(getString(R.string.feedback_collapse), GameAudio.COLLAPSE)
            }
        }
    }
    private fun feedback(text: String, sound: Int) {
        root.showFeedback(text)
        handler.removeCallbacks(hideFeedback)
        if (phase != COLLAPSING) handler.postDelayed(hideFeedback, 800)
        if (preferences.sound && hasWindowFocus()) audio.play(sound)
        if (preferences.vibration && hasWindowFocus()) {
            @Suppress("DEPRECATION")
            val vibrator = if (Build.VERSION.SDK_INT >= 31) getSystemService(VibratorManager::class.java).defaultVibrator
                else getSystemService(VIBRATOR_SERVICE) as Vibrator
            if (vibrator.hasVibrator()) vibrator.vibrate(VibrationEffect.createOneShot(if (sound == GameAudio.MISS || sound == GameAudio.COLLAPSE) 45 else 18, VibrationEffect.DEFAULT_AMPLITUDE))
        }
    }
    private fun overlayState() = GameOverlayState(phase, score, preferences.best, lives, combo, reason, failed, preferences.tutorialDone, creditsOpen)

    private fun renderUi() {
        backCallback.isEnabled = creditsOpen || (phase != MENU_PHASE && phase != -1 && !failed)
        root.keepScreenOn = (phase in SWINGING..4 || phase == COLLAPSING) && !failed
        root.render(overlayState())
    }
    private fun onUiAction(action: Int) {
        if (action == RELEASE) {
            if (phase != SWINGING || releasePending) return
            releasePending = true
        }
        nativeCommand(action)
        if (action == RELEASE && preferences.sound) audio.play(0)
    }
    private fun showSettings() {
        if (settingsDialog?.isShowing == true) return
        settingsDialog = AlertDialog.Builder(ContextThemeWrapper(this, R.style.ThemeOverlay_Cubbl_ArcadeDialog))
            .setTitle(R.string.settings_title)
            .setMultiChoiceItems(
                arrayOf(getString(R.string.settings_sound), getString(R.string.settings_vibration)),
                booleanArrayOf(preferences.sound, preferences.vibration)
            ) { _, which, _ ->
                if (which == 0) {
                    preferences.toggleSound()
                    if (!preferences.sound) audio.stop()
                } else preferences.toggleVibration()
            }
            .setPositiveButton(R.string.action_done, null)
            .setNeutralButton(R.string.credits_title) { _, _ -> showCredits(true) }
            .create().apply {
                setOnDismissListener { settingsDialog = null }
                show()
                root.styleSettings(this)
            }
    }
    private fun showCredits(open: Boolean) {
        settingsDialog?.dismiss()
        creditsOpen = open
        renderUi()
        if (!open) showSettings()
    }
    private fun dp(value: Int) = (value * resources.displayMetrics.density).toInt()
}
