package com.prammmoelabs.cubbl

import android.content.Context
import androidx.test.core.app.ActivityScenario
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.Espresso.pressBack
import androidx.test.espresso.action.ViewActions.click
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.matcher.ViewMatchers.*
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import org.junit.Assert.*
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class GameFlowTest {
    private fun prepareDevice(scenario: ActivityScenario<MainActivity>) {
        scenario.onActivity { activity ->
            activity.setShowWhenLocked(true)
            activity.setTurnScreenOn(true)
            activity.getSystemService(android.app.KeyguardManager::class.java)
                .requestDismissKeyguard(activity, null)
        }
    }
    private fun capture(name: String) {
        val context = InstrumentationRegistry.getInstrumentation().targetContext
        val image = InstrumentationRegistry.getInstrumentation().uiAutomation.takeScreenshot()
        checkNotNull(image)
        java.io.FileOutputStream(java.io.File(context.filesDir, name)).use {
            image.compress(android.graphics.Bitmap.CompressFormat.PNG, 100, it)
        }
        image.recycle()
    }
    private fun captureWorld(name: String, scenario: ActivityScenario<MainActivity>) {
        fun findSurface(view: android.view.View): android.view.SurfaceView? {
            if (view is android.view.SurfaceView) return view
            if (view is android.view.ViewGroup) for (i in 0 until view.childCount) {
                findSurface(view.getChildAt(i))?.let { return it }
            }
            return null
        }
        val copied = java.util.concurrent.CountDownLatch(1)
        var result = -1
        lateinit var image: android.graphics.Bitmap
        scenario.onActivity { activity ->
            val surface = checkNotNull(findSurface(activity.window.decorView))
            image = android.graphics.Bitmap.createBitmap(surface.width, surface.height, android.graphics.Bitmap.Config.ARGB_8888)
            android.view.PixelCopy.request(surface, image, { status -> result = status; copied.countDown() }, android.os.Handler(android.os.Looper.getMainLooper()))
        }
        assertTrue("Native framebuffer copy timed out", copied.await(10, java.util.concurrent.TimeUnit.SECONDS))
        try {
            assertEquals(android.view.PixelCopy.SUCCESS, result)
            val context = InstrumentationRegistry.getInstrumentation().targetContext
            java.io.FileOutputStream(java.io.File(context.filesDir, name)).use {
                image.compress(android.graphics.Bitmap.CompressFormat.PNG, 100, it)
            }
        } finally { image.recycle() }
    }
    private fun awaitText(value: String) {
        val deadline = System.currentTimeMillis() + 6000
        var failure: Throwable? = null
        var previousBounds: android.graphics.Rect? = null
        var stableSince = 0L
        while (System.currentTimeMillis() < deadline) {
            try {
                val matcher = if (value == "Pause") withContentDescription(value) else withText(value)
                val target = onView(matcher).let { if (value == "Settings") it.inRoot(androidx.test.espresso.matcher.RootMatchers.isDialog()) else it }
                target.check(matches(isDisplayed()))
                var bounds: android.graphics.Rect? = null
                target.check { view, error ->
                    if (error != null) throw error
                    if (view.isLaidOut && !view.isLayoutRequested && view.hasWindowFocus()) {
                        bounds = android.graphics.Rect().also { view.getGlobalVisibleRect(it) }
                    }
                }
                // Insets/window transitions can move a recreated panel after text becomes visible.
                val now = System.currentTimeMillis()
                if (bounds != null && bounds == previousBounds) {
                    if (now - stableSince >= 150) return
                } else {
                    previousBounds = bounds
                    stableSince = now
                }
            }
            catch (e: AssertionError) { failure = e }
            catch (e: androidx.test.espresso.NoMatchingViewException) { failure = e; previousBounds = null }
            Thread.sleep(50)
        }
        throw AssertionError("Timed out waiting for $value", failure)
    }
    @Test fun menuPlayPauseRestartAndSettings() {
        val command = InstrumentationRegistry.getInstrumentation().uiAutomation
            .executeShellCommand("settings put secure immersive_mode_confirmations confirmed")
        android.os.ParcelFileDescriptor.AutoCloseInputStream(command).use { it.readBytes() }
        val context = InstrumentationRegistry.getInstrumentation().targetContext
        context.getSharedPreferences("cubbl", Context.MODE_PRIVATE).edit().putBoolean("sound", true).putBoolean("vibration", true).commit()
        ActivityScenario.launch(MainActivity::class.java).use { scenario ->
            prepareDevice(scenario)
            awaitText("Start")
            capture("cubbl-menu.png")
            captureWorld("cubbl-world-home.png", scenario)
            onView(withText("Start")).perform(click())
            awaitText("Pause")
            onView(withContentDescription("Score 0")).check(matches(isDisplayed()))
            onView(withContentDescription("Lives 3 / 3")).check(matches(isDisplayed()))
            capture("cubbl-game.png")
            captureWorld("cubbl-world-tutorial.png", scenario)
            onView(withContentDescription("Pause")).perform(click())
            awaitText("Keep stacking")
            capture("cubbl-pause.png")
            onView(withText("Keep stacking")).perform(click())
            awaitText("Pause")
            scenario.moveToState(androidx.lifecycle.Lifecycle.State.CREATED)
            scenario.moveToState(androidx.lifecycle.Lifecycle.State.RESUMED)
            awaitText("Keep stacking")
            scenario.recreate()
            prepareDevice(scenario)
            awaitText("Keep stacking")
            onView(withText("Keep stacking")).perform(click())
            awaitText("Pause")
            onView(withContentDescription("Game area. Tap to drop a building.")).perform(click())
            Thread.sleep(1600)
            onView(withContentDescription("Pause")).check(matches(isDisplayed()))
            pressBack()
            awaitText("Keep stacking")
            onView(withText("Restart run")).perform(click())
            awaitText("Pause")
            pressBack(); awaitText("Keep stacking")
            onView(withText("Main menu")).perform(click())
            awaitText("Start")
            onView(withText("Sound")).check(androidx.test.espresso.assertion.ViewAssertions.doesNotExist())
            onView(withText("Vibration")).check(androidx.test.espresso.assertion.ViewAssertions.doesNotExist())
            onView(withContentDescription("Settings")).perform(click())
            awaitText("Settings")
            capture("cubbl-settings.png")
            onView(withText("Sound")).check(matches(isChecked())).perform(click())
            onView(withText("Sound")).check(matches(org.hamcrest.Matchers.not(isChecked())))
            onView(withText("Vibration")).check(matches(isChecked())).perform(click())
            scenario.recreate(); prepareDevice(scenario); awaitText("Settings")
            onView(withText("Sound")).check(matches(org.hamcrest.Matchers.not(isChecked())))
            onView(withText("Vibration")).check(matches(org.hamcrest.Matchers.not(isChecked())))
            onView(withText("Done")).perform(click())
            awaitText("Start")
            onView(withContentDescription("Settings")).perform(click())
            awaitText("Settings")
            onView(withText("Sound")).perform(click())
            onView(withText("Vibration")).perform(click())
            pressBack()
            awaitText("Start")
            onView(withText("Sound")).check(androidx.test.espresso.assertion.ViewAssertions.doesNotExist())
            assertTrue(context.getSharedPreferences("cubbl", Context.MODE_PRIVATE).getBoolean("sound", false))
            assertTrue(context.getSharedPreferences("cubbl", Context.MODE_PRIVATE).getBoolean("vibration", false))
        }
    }
    private fun snapshot(scenario: ActivityScenario<MainActivity>): LongArray {
        var result = longArrayOf()
        scenario.onActivity { activity ->
            result = MainActivity::class.java.getDeclaredMethod("nativeSnapshot").apply { isAccessible = true }
                .invoke(activity) as LongArray
        }
        return result
    }
    private fun command(scenario: ActivityScenario<MainActivity>, action: Int) {
        scenario.onActivity { activity ->
            MainActivity::class.java.getDeclaredMethod("nativeCommand", Int::class.javaPrimitiveType)
                .apply { isAccessible = true }.invoke(activity, action)
        }
    }
    private fun awaitSnapshot(scenario: ActivityScenario<MainActivity>, condition: (LongArray) -> Boolean): LongArray {
        val deadline = System.currentTimeMillis() + 8000
        while (System.currentTimeMillis() < deadline) {
            val state = snapshot(scenario)
            assertEquals(9, state.size)
            if (condition(state)) return state
            Thread.sleep(20)
        }
        throw AssertionError("Native state timed out: ${snapshot(scenario).contentToString()}")
    }
    @Test fun physicsLandingAndCollapseSurviveRecreation() {
        ActivityScenario.launch(MainActivity::class.java).use { scenario ->
            prepareDevice(scenario)
            command(scenario, 5); awaitText("Start")
            // Both commands enter the same queue, so the first drop starts exactly at the center.
            command(scenario, 0); command(scenario, 1)
            awaitSnapshot(scenario) { it[0] == 3L }
            command(scenario, 2)
            awaitText("Keep stacking")
            val landing = snapshot(scenario)
            assertEquals(0L, landing[4])
            scenario.recreate(); prepareDevice(scenario); awaitText("Keep stacking")
            Thread.sleep(400)
            assertEquals(0L, snapshot(scenario)[4])
            onView(withText("Keep stacking")).perform(click())
            awaitSnapshot(scenario) { it[4] == 1L }
            command(scenario, 4)
            awaitSnapshot(scenario) { it[0] == 1L && it[4] == 0L }
            // Build an offset tower: the first house now rests safely against solid ground.
            command(scenario, 1)
            awaitSnapshot(scenario) { it[4] == 1L && it[0] == 1L }
            val collapseDeadline = System.currentTimeMillis() + 20000
            while (snapshot(scenario)[0] != 7L && System.currentTimeMillis() < collapseDeadline) {
                val current = snapshot(scenario)
                if (current[0] == 1L) {
                    Thread.sleep(70)
                    command(scenario, 1)
                    awaitSnapshot(scenario) { it[0] != 1L }
                } else Thread.sleep(20)
            }
            val collapsing = awaitSnapshot(scenario) { it[0] == 7L }
            command(scenario, 2); awaitText("Keep stacking")
            scenario.recreate(); prepareDevice(scenario); awaitText("Keep stacking")
            Thread.sleep(1700)
            assertEquals(5L, snapshot(scenario)[0])
            assertEquals(collapsing[1], snapshot(scenario)[1])
            onView(withText("Keep stacking")).perform(click())
            awaitText("Timber!")
            capture("cubbl-tilted.png")
            val ended = awaitSnapshot(scenario) { it[0] == 6L }
            awaitText("Tower down!")
            assertEquals(2L, ended[8]); assertEquals(3L, ended[3])
            assertEquals(collapsing[1], ended[1])
            awaitText("Stack again")
            capture("cubbl-collapse.png")
            captureWorld("cubbl-world-collapse.png", scenario)
            onView(withText("Stack again")).perform(click())
            val restarted = awaitSnapshot(scenario) { it[0] == 1L && it[4] == 0L }
            assertEquals(0L, restarted[8]); assertEquals(3L, restarted[3])
            awaitText("Pause")
            onView(withContentDescription("Game area. Tap to drop a building.")).perform(
                object : androidx.test.espresso.ViewAction {
                    override fun getConstraints(): org.hamcrest.Matcher<android.view.View> = isDisplayed()
                    override fun getDescription() = "Cancel a drop gesture"
                    override fun perform(controller: androidx.test.espresso.UiController, view: android.view.View) {
                        val now = android.os.SystemClock.uptimeMillis()
                        for (action in intArrayOf(android.view.MotionEvent.ACTION_DOWN, android.view.MotionEvent.ACTION_CANCEL)) {
                            val event = android.view.MotionEvent.obtain(now, now, action, view.width / 2f, view.height / 2f, 0)
                            view.dispatchTouchEvent(event); event.recycle()
                        }
                        controller.loopMainThreadForAtLeast(100)
                    }
                }
            )
            assertEquals(1L, snapshot(scenario)[0])
            assertEquals(0L, snapshot(scenario)[4])
            command(scenario, 5); awaitText("Start")
        }
    }
    @Test fun canalRemainsVisibleWhenCameraLeavesGround() {
        ActivityScenario.launch(MainActivity::class.java).use { scenario ->
            prepareDevice(scenario)
            lateinit var activity: MainActivity
            scenario.onActivity { activity = it }
            val send = MainActivity::class.java.getDeclaredMethod("nativeCommand", Int::class.javaPrimitiveType).apply { isAccessible = true }
            val read = MainActivity::class.java.getDeclaredMethod("nativeSnapshot").apply { isAccessible = true }
            // JNI uses a thread-safe queue/snapshot. Poll directly so UI idle waits cannot delay centered drops.
            send.invoke(activity, 4); send.invoke(activity, 1)
            for (floors in 1L..8L) {
                val deadline = System.currentTimeMillis() + 8000
                var state: LongArray
                do {
                    state = read.invoke(activity) as LongArray
                    assertEquals(0L, state[7])
                    assertEquals(3L, state[3])
                    check(System.currentTimeMillis() < deadline) { "Centered stack timed out: ${state.contentToString()}" }
                    if (state[4] != floors || state[0] != 1L) Thread.sleep(5)
                } while (state[4] != floors || state[0] != 1L)
                if (floors < 8) send.invoke(activity, 1)
            }
            Thread.sleep(350)
            capture("cubbl-canal.png")
            captureWorld("cubbl-world-tower.png", scenario)
            command(scenario, 5); awaitText("Start")
        }
    }
    private fun skyColor(): IntArray {
        val image = checkNotNull(InstrumentationRegistry.getInstrumentation().uiAutomation.takeScreenshot())
        val color = IntArray(3)
        for (row in 0..7) for (col in 0..7) {
            val pixel = image.getPixel((image.width * .08f).toInt() + col * 3, (image.height * .035f).toInt() + row * 3)
            color[0] += android.graphics.Color.red(pixel)
            color[1] += android.graphics.Color.green(pixel)
            color[2] += android.graphics.Color.blue(pixel)
        }
        image.recycle()
        return IntArray(3) { color[it] / 64 }
    }
    @Test fun newRunsChangeSkyAndRecreationKeepsAtmosphere() {
        ActivityScenario.launch(MainActivity::class.java).use { scenario ->
            prepareDevice(scenario)
            command(scenario, 5); awaitText("Start")
            var previous: IntArray? = null
            for (run in 0..5) {
                // Exercise both Start from home and Restart from pause.
                command(scenario, if (run % 2 == 0) 0 else 4)
                awaitSnapshot(scenario) { it[0] == 1L }
                assertEquals(0L, snapshot(scenario)[7])
                command(scenario, 2); awaitText("Keep stacking")
                val current = skyColor()
                previous?.let { old -> assertTrue("New run reused its sky", current.indices.sumOf { kotlin.math.abs(current[it] - old[it]) } > 40) }
                if (run == 0) {
                    scenario.recreate(); prepareDevice(scenario); awaitText("Keep stacking")
                    val recreated = skyColor()
                    assertTrue("Recreation changed the sky", current.indices.sumOf { kotlin.math.abs(current[it] - recreated[it]) } < 8)
                }
                onView(withText("Keep stacking")).perform(click()); awaitText("Pause")
                val resumed = skyColor()
                assertTrue("Resume changed the sky", current.indices.sumOf { kotlin.math.abs(current[it] - resumed[it]) } < 8)
                val name = when {
                    current[1] < 50 -> "night"
                    current[1] > 150 -> "morning"
                    current[1] > 112 -> "day"
                    else -> "sunset"
                }
                capture("cubbl-atmosphere-$name.png")
                command(scenario, 2); awaitText("Keep stacking")
                previous = current
                if (run % 2 == 1) { command(scenario, 5); awaitText("Start") }
            }
            command(scenario, 5); awaitText("Start")
        }
    }
    @Test fun preferencesNeverLowerRecordAndSurviveRecreation() {
        val context = InstrumentationRegistry.getInstrumentation().targetContext
        val file = context.getSharedPreferences("test-record", Context.MODE_PRIVATE)
        file.edit().clear().commit()
        val first = PlayerPreferences(file)
        first.record(500); first.record(20); first.completeTutorial(); first.toggleSound()
        val second = PlayerPreferences(file)
        assertEquals(500L, second.best)
        assertTrue(second.tutorialDone); assertFalse(second.sound)
        file.edit().putString("best", "broken").commit()
        assertEquals(0L, PlayerPreferences(file).best)
        file.edit().clear().commit()
    }
}
