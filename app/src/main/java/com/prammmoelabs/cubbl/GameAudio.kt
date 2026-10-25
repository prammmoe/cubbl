package com.prammmoelabs.cubbl

import android.content.Context
import android.media.AudioAttributes
import android.media.SoundPool

class GameAudio(context: Context) {
    companion object {
        const val PERFECT = 2
        const val STREAK = 5
        internal fun perfectSound(previousCombo: Int, combo: Int) =
            if (previousCombo < 2 && combo >= 2) STREAK else PERFECT
        const val MISS = 3
        const val COLLAPSE = 4
    }
    private val pool = SoundPool.Builder().setMaxStreams(4).setAudioAttributes(
        AudioAttributes.Builder().setUsage(AudioAttributes.USAGE_GAME)
            .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION).build()
    ).build()
    private val ready = mutableSetOf<Int>()
    private val streams = ArrayDeque<Int>()
    private val sounds: IntArray
    init {
        pool.setOnLoadCompleteListener { _, id, status -> if (status == 0) ready.add(id) }
        sounds = intArrayOf(
            pool.load(context, R.raw.drop, 1), pool.load(context, R.raw.land, 1),
            pool.load(context, R.raw.perfect, 1), pool.load(context, R.raw.miss, 1),
            pool.load(context, R.raw.collapse, 1), pool.load(context, R.raw.streak, 1)
        )
    }
    fun play(index: Int) {
        val id = sounds[index]
        if (id in ready) {
            if (streams.size >= 8) streams.removeFirst()
            streams.addLast(pool.play(id, .55f, .55f, 1, 0, 1f))
        }
    }
    fun stop() { streams.forEach { pool.stop(it) }; streams.clear() }
    fun release() { stop(); pool.release() }
}
