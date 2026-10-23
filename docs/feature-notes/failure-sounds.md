# Collapse and miss sounds

## 2026-10-23

Added distinct bundled failure effects using CC0 Kenney Impact Sounds and OpenGameArt Breaking Rock recordings. Miss uses a short 0.5-second thud; collapse uses a 1.4-second sequence of pitched rock cracks and material impacts. Both are small mono WAV assets with fades and normalized peaks. Provenance, source hashes, upstream license and processing filters are recorded in [audio credits](../audio/CREDITS.md).

The collapse event now selects its own SoundPool sample instead of reusing miss. Both retain the longer failure vibration. Existing event-sequence deduplication, Sound preference, focus gating, pause stop, UI and physics are unchanged. No Git branch is available in this workspace.

Verification:
- Both assets are mono 44.1 kHz PCM16 WAV: miss 44,144 bytes / 0.50 s, collapse 123,524 bytes / 1.40 s. Peak 0.85, no clipped samples.
- `:app:assembleDebug :app:testDebugUnitTest :app:lintDebug :app:assembleDebugAndroidTest` passed. Two JVM tests passed; lint has zero errors and the same nine SDK/dependency/orientation warnings.
- Pixel 8 API 37 emulator: all seven instrumentation tests passed in 57.053 seconds. New audio test verifies clip durations, successful asynchronous loads and accepted distinct SoundPool playback handles at zero gain; existing tests cover collapse, pause/recreation and settings persistence.
- SoundPool compatibility was tested; subjective listening/balance on a physical phone was not measured.
- APK: `app/build/outputs/apk/debug/app-debug.apk`.
