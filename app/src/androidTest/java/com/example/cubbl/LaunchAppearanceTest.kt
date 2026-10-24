package com.example.cubbl

import android.graphics.Color
import android.os.Build
import android.util.TypedValue
import android.view.View
import androidx.test.core.app.ActivityScenario
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.matcher.ViewMatchers.isDisplayed
import androidx.test.espresso.matcher.ViewMatchers.withText
import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class LaunchAppearanceTest {
    @Test fun sunsetLaunchReachesPlayableHomeAfterRecreation() {
        ActivityScenario.launch(MainActivity::class.java).use { scenario ->
            repeat(2) { launch ->
                if (launch > 0) scenario.recreate()
                scenario.onActivity { activity ->
                    if (Build.VERSION.SDK_INT >= 31) {
                        val color = TypedValue()
                        assertTrue(activity.theme.resolveAttribute(android.R.attr.windowSplashScreenBackground, color, true))
                        assertEquals(Color.rgb(238, 154, 117), color.data)
                    }
                }
                val deadline = System.currentTimeMillis() + 6000
                var covered = true
                while (covered && System.currentTimeMillis() < deadline) {
                    scenario.onActivity { covered = it.window.decorView.findViewWithTag<View>("launch_city") != null }
                    if (covered) Thread.sleep(50)
                }
                scenario.onActivity {
                    assertNull("City launch cover must leave when the native scene is ready", it.window.decorView.findViewWithTag<View>("launch_city"))
                }
                onView(withText("Start")).check(matches(isDisplayed()))
            }
        }
    }
}
