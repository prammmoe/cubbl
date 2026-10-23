package com.example.cubbl

import org.junit.Assert.assertEquals
import org.junit.Test

class StreakSoundTest {
    @Test fun chimeOnlyReplacesPerfectWhenStreakActivates() {
        assertEquals(GameAudio.PERFECT, GameAudio.perfectSound(0, 1))
        assertEquals(GameAudio.STREAK, GameAudio.perfectSound(1, 2))
        assertEquals(GameAudio.PERFECT, GameAudio.perfectSound(2, 2))
        assertEquals(GameAudio.PERFECT, GameAudio.perfectSound(2, 3))
        assertEquals(GameAudio.STREAK, GameAudio.perfectSound(0, 3))
        assertEquals(GameAudio.PERFECT, GameAudio.perfectSound(0, 1))
        assertEquals(GameAudio.STREAK, GameAudio.perfectSound(1, 2))
    }
}
