package com.prammmoelabs.cubbl

import android.media.AudioAttributes
import android.media.MediaMetadataRetriever
import android.media.SoundPool
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import org.junit.Assert.*
import org.junit.Test
import org.junit.runner.RunWith
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit

@RunWith(AndroidJUnit4::class)
class FailureAudioTest {
    @Test fun bundledEffectsDecodeAndPlayAsDistinctShortEffects() {
        val context = InstrumentationRegistry.getInstrumentation().targetContext
        val resources = listOf(R.raw.miss, R.raw.collapse, R.raw.streak)
        val durations = resources.map { resource ->
            context.resources.openRawResourceFd(resource).use { asset ->
                MediaMetadataRetriever().use { reader ->
                    reader.setDataSource(asset.fileDescriptor, asset.startOffset, asset.length)
                    reader.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)!!.toLong()
                }
            }
        }
        assertTrue(durations[0] in 450L..550L)
        assertTrue(durations[1] in 1300L..1500L)
        assertTrue(durations[2] in 450L..510L)
        val pool = SoundPool.Builder().setMaxStreams(2).setAudioAttributes(
            AudioAttributes.Builder().setUsage(AudioAttributes.USAGE_GAME)
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION).build()
        ).build()
        try {
            val ready = CountDownLatch(resources.size)
            val statuses = ConcurrentHashMap<Int, Int>()
            pool.setOnLoadCompleteListener { _, id, status -> statuses[id] = status; ready.countDown() }
            val ids = resources.map { pool.load(context, it, 1) }
            assertTrue(ids.all { it > 0 }); assertEquals(resources.size, ids.toSet().size)
            assertTrue("Effects did not load", ready.await(5, TimeUnit.SECONDS))
            ids.forEach { id ->
                assertEquals(0, statuses[id])
                val stream = pool.play(id, 0f, 0f, 1, 0, 1f)
                assertTrue("SoundPool rejected playback", stream > 0)
                pool.stop(stream)
            }
        } finally { pool.release() }
    }
}
