package com.prammmoelabs.cubbl

import org.junit.Assert.assertEquals
import org.junit.Test

class PlayerPreferencesTest {
    @Test fun corruptBestUsesSafeDefault() {
        assertEquals(0L, PlayerPreferences.sanitizeBest(null))
        assertEquals(0L, PlayerPreferences.sanitizeBest("900"))
        assertEquals(0L, PlayerPreferences.sanitizeBest(-1L))
        assertEquals(900L, PlayerPreferences.sanitizeBest(900L))
    }
}
