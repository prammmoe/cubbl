# Streak Feedback Implementation Notes

Branch: unavailable (workspace has no Git repository).

## 2026-10-23 — Streak above placement feedback

### Changed
- Streak and Perfect share one 28sp Baloo 2 extra-bold/ivory/shadow text helper. Removed Streak's sage badge, forced uppercase and letter spacing; it now sits directly above feedback in a vertical stack below the HUD.
- Added an original rising 480ms chime, generated offline with a standard-library script in docs/audio/generate-streak.py.
- A newly activated combo (below 2 → 2 or greater) selects the chime instead of the normal Perfect sound. Continuing streaks retain Perfect audio; resetting and earning another streak can trigger the chime again.
- Selection uses the existing deduplicated Perfect-event branch, Sound preference, focus/pause handling and SoundPool. No production dependency or JNI change.

### Verification
- Added a Streak-above-Perfect screenshot, sound selection regression checks, and native decoder/SoundPool coverage for the new WAV.
- `:app:recordPaparazziDebug` / `:app:verifyPaparazziDebug`: passed, 14 JVM tests with zero failures/errors. Reviewed Streak-above-Perfect, persistent Streak, and collapse screenshots.
- `:app:assembleDebug :app:assembleDebugAndroidTest :app:assembleRelease :app:lintDebug`: passed.
- WAV check: 480ms mono PCM16 / 22.05 kHz, peak 0.70, smooth attack/release; reproducible script stored with audio credits.
- Android 37 / Pixel 8 temporary emulator with host GPU: `FailureAudioTest` plus `GameFlowTest#menuPlayPauseRestartAndSettings` passed (2 tests, 17.806 seconds). Verifies all three impact/chime WAVs decode, load and receive valid playback stream IDs; also checks navigation, pause, settings and recreation. Temporary emulator removed afterward.
- No physical-device listening or performance measurement performed.
