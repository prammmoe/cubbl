package com.example.cubbl

import android.content.SharedPreferences
import androidx.core.content.edit

class PlayerPreferences(private val preferences: SharedPreferences) {
    var best: Long = sanitizeBest(preferences.all["best"])
        private set
    var sound: Boolean = preferences.all["sound"] as? Boolean ?: true
        private set
    var vibration: Boolean = preferences.all["vibration"] as? Boolean ?: true
        private set
    var tutorialDone: Boolean = preferences.all["tutorial"] as? Boolean ?: false
        private set

    fun record(score: Long) {
        if (score > best) {
            best = score
            preferences.edit { putLong("best", best) }
        }
    }
    fun toggleSound() { sound = !sound; preferences.edit { putBoolean("sound", sound) } }
    fun toggleVibration() { vibration = !vibration; preferences.edit { putBoolean("vibration", vibration) } }
    fun completeTutorial() {
        if (!tutorialDone) { tutorialDone = true; preferences.edit { putBoolean("tutorial", true) } }
    }
    companion object {
        fun sanitizeBest(value: Any?): Long = ((value as? Long) ?: 0L).coerceAtLeast(0)
    }
}
