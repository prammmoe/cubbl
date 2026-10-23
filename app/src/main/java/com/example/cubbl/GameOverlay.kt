package com.example.cubbl

import android.content.Context
import android.graphics.Typeface
import android.graphics.drawable.GradientDrawable
import android.graphics.drawable.LayerDrawable
import android.graphics.drawable.StateListDrawable
import android.util.TypedValue
import android.view.Gravity
import android.view.View
import android.widget.Button
import android.widget.FrameLayout
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AlertDialog

internal const val START = 0
internal const val RELEASE = 1
internal const val PAUSE = 2
internal const val RESUME = 3
internal const val RESTART = 4
internal const val MENU = 5
internal const val MENU_PHASE = 0
internal const val SWINGING = 1
internal const val PAUSED = 5
internal const val GAME_OVER = 6
internal const val COLLAPSING = 7

internal data class GameOverlayState(
    val phase: Int = MENU_PHASE,
    val score: Long = 0,
    val best: Long = 0,
    val lives: Int = 3,
    val combo: Int = 0,
    val reason: Int = 0,
    val failed: Boolean = false,
    val tutorialDone: Boolean = false
)

internal class GameOverlay(
    context: Context,
    private val onAction: (Int) -> Unit,
    private val onSettings: () -> Unit,
    private val onClose: () -> Unit
) : FrameLayout(context) {
    private val dark = context.getColor(R.color.ui_dark)
    private val ivory = context.getColor(R.color.ui_surface)
    private val sage = context.getColor(R.color.ui_secondary)
    private val terracotta = context.getColor(R.color.ui_terracotta)
    private val terracottaDark = context.getColor(R.color.ui_terracotta_dark)
    private val heart = context.getColor(R.color.ui_heart)
    private val font = resources.getFont(R.font.baloo_2)
    private var state = GameOverlayState()
    private val root get() = this
    private val phase get() = state.phase
    private val score get() = state.score
    private val lives get() = state.lives
    private val combo get() = state.combo
    private val reason get() = state.reason
    private val failed get() = state.failed
    private fun getString(id: Int, vararg args: Any) = context.getString(id, *args)

    init { setPadding(dp(20), dp(20), dp(20), dp(24)) }

    fun showFeedback(value: String) {
        feedbackView?.text = value
        feedbackView?.announceForAccessibility(value)
    }
    fun clearFeedback() { feedbackView?.text = "" }
    fun styleSettings(dialog: AlertDialog) {
        dialog.window?.setBackgroundDrawable(surface(ivory, dark, 6))
        dialog.getButton(AlertDialog.BUTTON_POSITIVE).apply {
            setTextColor(dark)
            typeface = Typeface.create(font, 700, false)
        }
    }
    private var scoreView: TextView? = null
    private var bestView: TextView? = null
    private var livesGroup: LinearLayout? = null
    private var streakView: TextView? = null
    private val heartViews = mutableListOf<ImageView>()
    private var feedbackView: TextView? = null
    private var hintView: TextView? = null
    fun render(value: GameOverlayState) {
        state = value
        root.removeAllViews()
        scoreView = null; bestView = null; livesGroup = null; streakView = null
        heartViews.clear(); feedbackView = null; hintView = null
        root.setOnClickListener(null)
        root.isClickable = false
        root.contentDescription = null
        if (failed) {
            val panel = panel()
            panel.addView(text(getString(R.string.display_error_title), 26))
            panel.addView(text(getString(R.string.display_error_message), 17))
            panel.addView(button(getString(R.string.action_close)) { onClose() })
            return
        }
        when (phase) {
            -1, MENU_PHASE -> {
                val panel = panel()
                panel.addView(text(getString(R.string.app_name), 48).apply { letterSpacing = .06f })
                panel.addView(text(getString(R.string.tagline), 16).apply { setTextColor(dark) })
                panel.addView(text(getString(R.string.best, state.best), 22).apply { setTextColor(dark) })
                panel.addView(button(getString(R.string.action_start), primary = true) { onAction(START) })
                val config = iconButton(ArcadeIcon.Symbol.SETTINGS, getString(R.string.settings_title)) { onSettings() }
                root.addView(config, FrameLayout.LayoutParams(dp(48), dp(48), Gravity.TOP or Gravity.END))
            }
            PAUSED -> {
                val panel = panel()
                panel.addView(text(getString(R.string.paused_title), 34))
                panel.addView(text(getString(R.string.score, score), 22))
                panel.addView(button(getString(R.string.action_resume), primary = true) { onAction(RESUME) })
                panel.addView(button(getString(R.string.action_restart)) { onAction(RESTART) })
                panel.addView(button(getString(R.string.action_menu)) { onAction(MENU) })
            }
            GAME_OVER -> {
                val panel = panel()
                panel.addView(text(getString(R.string.game_over_title), 32))
                panel.addView(text(getString(if (reason == 2) R.string.game_over_collapse else R.string.game_over_lives), 22))
                panel.addView(text(getString(R.string.score_record, score, state.best), 24))
                panel.addView(button(getString(R.string.action_play_again), primary = true) { onAction(RESTART) })
                panel.addView(button(getString(R.string.action_menu)) { onAction(MENU) })
            }
            else -> {
                root.contentDescription = getString(R.string.game_area_description)
                root.setOnClickListener {
                    if (phase == SWINGING) onAction(RELEASE)
                }
                val hud = LinearLayout(context).apply { orientation = LinearLayout.VERTICAL }
                val row = LinearLayout(context).apply { orientation = LinearLayout.HORIZONTAL; gravity = Gravity.TOP }
                val lifeCard = LinearLayout(context).apply {
                    orientation = LinearLayout.VERTICAL; gravity = Gravity.CENTER
                    background = surface(sage, dark)
                    setPadding(dp(8), dp(8), dp(8), dp(12))
                    importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_YES
                }
                lifeCard.addView(text(getString(R.string.hud_lives), 13).apply {
                    setPadding(0, 0, 0, dp(4)); isAllCaps = true; letterSpacing = .1f
                    importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO
                })
                val hearts = LinearLayout(context).apply { gravity = Gravity.CENTER }
                repeat(3) { index ->
                    val heart = ImageView(context).apply { importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO }
                    heartViews.add(heart)
                    hearts.addView(heart, LinearLayout.LayoutParams(dp(22), dp(24)).apply { if (index > 0) leftMargin = dp(3) })
                }
                lifeCard.addView(hearts); livesGroup = lifeCard
                row.addView(lifeCard, LinearLayout.LayoutParams(dp(92), -2))
                val scoreCard = LinearLayout(context).apply {
                    orientation = LinearLayout.VERTICAL; gravity = Gravity.CENTER
                    background = surface(sage, dark)
                    setPadding(dp(8), dp(6), dp(8), dp(10))
                }
                scoreCard.addView(text(getString(R.string.hud_score), 13).apply {
                    isAllCaps = true; letterSpacing = .12f; setPadding(0, 0, 0, 0)
                    importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO
                }, LinearLayout.LayoutParams(-1, dp(18)))
                scoreView = text(getString(R.string.hud_score_value, score), 42).apply {
                    setPadding(0, 0, 0, 0); setSingleLine()
                    setAutoSizeTextTypeUniformWithConfiguration(10, 42, 1, TypedValue.COMPLEX_UNIT_SP)
                }
                scoreCard.addView(scoreView, LinearLayout.LayoutParams(-1, dp(56)))
                bestView = text(getString(R.string.best, state.best), 15).apply {
                    setPadding(0, 0, 0, 0); setTextColor(dark); setSingleLine()
                    setAutoSizeTextTypeUniformWithConfiguration(10, 15, 1, TypedValue.COMPLEX_UNIT_SP)
                }
                scoreCard.addView(bestView, LinearLayout.LayoutParams(-1, dp(22)))
                row.addView(scoreCard, LinearLayout.LayoutParams(0, dp(112), 1f).apply { setMargins(dp(8), 0, dp(8), 0) })
                row.addView(iconButton(ArcadeIcon.Symbol.PAUSE, getString(R.string.action_pause)) { onAction(PAUSE) }, LinearLayout.LayoutParams(dp(52), dp(52)))
                hud.addView(row)
                root.addView(hud, FrameLayout.LayoutParams(-1, -2, Gravity.TOP))
                val reactions = LinearLayout(context).apply {
                    orientation = LinearLayout.VERTICAL; gravity = Gravity.CENTER_HORIZONTAL
                }
                streakView = feedbackText("")
                reactions.addView(streakView, LinearLayout.LayoutParams(-1, -2))
                feedbackView = feedbackText(if (phase == COLLAPSING) getString(R.string.feedback_collapse) else "").apply {
                    importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO
                }
                reactions.addView(feedbackView, LinearLayout.LayoutParams(-1, -2))
                root.addView(reactions, FrameLayout.LayoutParams(-1, -2, Gravity.TOP).apply { topMargin = dp(136) })
                hintView = text(getString(R.string.tutorial), 17).apply {
                    gravity = Gravity.CENTER
                    background = surface(sage, dark)
                }
                root.addView(hintView, FrameLayout.LayoutParams(-1, -2, Gravity.BOTTOM))
            }
        }
        update(state)
    }
    fun update(value: GameOverlayState) {
        state = value
        scoreView?.text = getString(R.string.hud_score_value, score)
        scoreView?.contentDescription = getString(R.string.score, score)
        bestView?.text = getString(R.string.best, state.best)
        livesGroup?.contentDescription = getString(R.string.lives, lives)
        heartViews.forEachIndexed { index, view ->
            view.setImageDrawable(ArcadeIcon(ArcadeIcon.Symbol.HEART, if (index < lives) heart else dark, index < lives))
        }
        streakView?.text = getString(R.string.hud_streak, combo)
        streakView?.visibility = if (combo > 1) View.VISIBLE else View.GONE
        hintView?.visibility = if (state.tutorialDone) View.GONE else View.VISIBLE
    }
    private fun panel(): LinearLayout {
        val content = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL; gravity = Gravity.CENTER
            setPadding(dp(20), dp(20), dp(20), dp(20))
            background = surface(ivory, dark)
        }
        val scroll = android.widget.ScrollView(context).apply {
            isFillViewport = false
            addView(content)
        }
        root.addView(scroll, FrameLayout.LayoutParams(-1, -2, Gravity.CENTER).apply { setMargins(dp(8), dp(8), dp(8), dp(8)) })
        return content
    }
    private fun feedbackText(value: String): TextView = text(value, 28).apply {
        setTextColor(ivory)
        setShadowLayer(dp(3).toFloat(), 0f, dp(2).toFloat(), dark)
        setPadding(dp(12), 0, dp(12), 0)
    }
    private fun text(value: String, size: Int): TextView = TextView(context).apply {
        text = value; textSize = size.toFloat(); setTextColor(dark)
        gravity = Gravity.CENTER
        typeface = Typeface.create(font, if (size >= 26) 800 else 500, false)
        includeFontPadding = false
        setPadding(dp(12), dp(10), dp(12), dp(10))
    }
    private fun button(value: String, primary: Boolean = false, action: () -> Unit): Button = Button(context).apply {
        text = value; textSize = 20f; isAllCaps = true; minHeight = dp(56); letterSpacing = .04f
        typeface = Typeface.create(font, 700, false)
        includeFontPadding = false
        setTextColor(if (primary) terracottaDark else dark)
        backgroundTintList = null; stateListAnimator = null
        background = controlBackground(if (primary) terracotta else sage, if (primary) terracottaDark else dark)
        setPadding(dp(12), dp(12), dp(12), dp(16))
        setOnClickListener { action() }
        layoutParams = LinearLayout.LayoutParams(-1, -2).apply { topMargin = dp(12) }
    }
    private fun iconButton(symbol: ArcadeIcon.Symbol, label: String, action: () -> Unit) = ImageButton(context).apply {
        setImageDrawable(ArcadeIcon(symbol, dark))
        scaleType = ImageView.ScaleType.FIT_CENTER
        contentDescription = label; tooltipText = label
        backgroundTintList = null; stateListAnimator = null
        background = controlBackground(if (symbol == ArcadeIcon.Symbol.SETTINGS) ivory else sage, dark)
        setPadding(dp(12), dp(10), dp(12), dp(14))
        minimumWidth = dp(48); minimumHeight = dp(48)
        setOnClickListener { action() }
    }
    private fun surface(fill: Int, edge: Int, depth: Int = 4, pressed: Boolean = false): LayerDrawable {
        fun plate(color: Int) = GradientDrawable().apply {
            setColor(color); cornerRadius = dp(16).toFloat()
        }
        return LayerDrawable(arrayOf(plate(if (pressed) android.graphics.Color.TRANSPARENT else edge), plate(fill))).apply {
            setLayerInset(1, 0, if (pressed) dp(depth) else 0, 0, if (pressed) 0 else dp(depth))
        }
    }
    private fun controlBackground(fill: Int, edge: Int) = StateListDrawable().apply {
        addState(intArrayOf(android.R.attr.state_pressed), surface(fill, edge, pressed = true))
        addState(intArrayOf(android.R.attr.state_focused), surface(fill, edge, pressed = true).apply {
            (getDrawable(1) as GradientDrawable).setStroke(dp(2), edge)
        })
        addState(intArrayOf(), surface(fill, edge))
    }
    private fun dp(value: Int) = (value * resources.displayMetrics.density).toInt()
}
