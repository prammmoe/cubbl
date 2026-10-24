package com.example.cubbl

import androidx.test.core.app.ActivityScenario
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.Espresso.pressBack
import androidx.test.espresso.action.ViewActions.click
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.assertion.ViewAssertions.doesNotExist
import androidx.test.espresso.matcher.ViewMatchers.isDisplayed
import androidx.test.espresso.matcher.ViewMatchers.withText
import androidx.test.espresso.matcher.ViewMatchers.withContentDescription
import androidx.test.ext.junit.runners.AndroidJUnit4
import android.text.Spanned
import android.text.style.URLSpan
import android.widget.TextView
import org.junit.Assert.assertEquals
import org.junit.Test
import org.junit.runner.RunWith
import org.hamcrest.Matchers.containsString

@RunWith(AndroidJUnit4::class)
class CreditsFlowTest {
    @Test fun settingsCreditsSurviveRecreationAndReturnToSettings() {
        ActivityScenario.launch(MainActivity::class.java).use { scenario ->
            onView(withText("Credits")).check(doesNotExist())
            onView(withContentDescription("Settings")).perform(click())
            onView(withText("Credits")).perform(click())
            onView(withText("Developer")).check(matches(isDisplayed()))
            onView(withText("Prammmoe")).check(matches(isDisplayed()))
            onView(withText("https://prammmoe.space/")).check { view, error ->
                if (error != null) throw error
                val text = (view as TextView).text as Spanned
                assertEquals("https://prammmoe.space/", text.getSpans(0, text.length, URLSpan::class.java).single().url)
            }
            onView(withText("Asset licenses")).perform(click())
            onView(withText(containsString("Copyright (c) 2022 Erin Catto"))).check(matches(isDisplayed()))
            onView(withText("Done")).perform(click())
            scenario.recreate()
            onView(withText("Prammmoe")).check(matches(isDisplayed()))
            pressBack()
            onView(withText("Sound")).check(matches(isDisplayed()))
            onView(withText("Credits")).perform(click())
            onView(withText("Back")).perform(click())
            onView(withText("Sound")).check(matches(isDisplayed()))
            onView(withText("Done")).perform(click())
            onView(withText("Start")).check(matches(isDisplayed()))
            onView(withText("Credits")).check(doesNotExist())
        }
    }
}
