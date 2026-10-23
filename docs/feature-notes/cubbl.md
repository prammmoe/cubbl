# Cubbl implementation notes

Workspace: no Git repository present; no branches, commits or worktrees created.

## 2026-10-22 — Offline stacking game

### Changed
- C++ domain, procedural cube renderer, fixed-step clock, JNI command queue and Android UI/storage/audio adapters.
- Three lives, arcade snap, combo scoring, capped speed, three building templates, camera follow and visible-floor rendering.

### Decisions
- Native Game lives separately from EGL resources, so surface teardown pauses rather than discarding the session.
- Android Views provide accessible text/buttons without a custom text renderer.
- Existing SDK/dependency versions retained. No online services or monetization.
- Task 1 baseline passed after granting Gradle access to its local cache. No Git-specific tooling applies to this workspace.
- Latest Android target requires OnBackPressedDispatcher; legacy onBackPressed failed lint and was replaced.
- Host FrameClock is shared with runtime so 30/60/120Hz and resume timing can be checked without Android.

### Verification
- Host CTest currently passes including 1,000 placements, geometry and clock checks.
- Final build: `:app:assembleDebug :app:assembleDebugAndroidTest :app:testDebugUnitTest :app:lintDebug` passed. APK: `app/build/outputs/apk/debug/app-debug.apk` (17 MB, four ABIs).
- Final lint: 0 errors, 9 warnings. Warnings concern retained dependency versions, intentional portrait policy, and an API 33+ Back attribute ignored on older supported Android versions. No check was suppressed.
- Final direct instrumentation on isolated CubblTest/API 35: `am instrument -w com.example.cubbl.test/androidx.test.runner.AndroidJUnitRunner` reports `OK (3 tests)` in 9.029 seconds. Covers menu, pause/resume, background, Activity recreation, release, Back, restart, sound and real SharedPreferences validation.
- Additional device script passed three timed misses -> game over -> Main Lagi -> pause -> menu; vibration preference survived force-stop/relaunch. Record 30 remained visible after process restart.
- Screenshots: `screenshots/playing.png`, `screenshots/game-over.png`, `screenshots/menu.png`; reviewed visually after capture. No Cubbl render errors, fatal signals or AndroidRuntime fatal exceptions appeared in captured runtime logs.
- Gradle connected-test discovery could not use existing Pixel 8 (property timeout); the same instrumentation APK was therefore run directly on the isolated AVD instead of hiding or skipping tests.
- Physical 60 FPS, audible playback on real hardware, long-run GPU memory measurement and deliberate GPU context-loss injection are not verified. Host stress covers 1,000 placements; renderer culling and shared GPU-resource ownership were reviewed statically.
- Review regression: original palette arithmetic mapped consecutive small seeds to the same color; a failing variation test caught it. Replaced it with direct seed modulo palette size, matching the 15 cached variant/color combinations.
- UI text/background contrast measured at 7.31:1 and 10.93:1. Decorative pastel geometry does not carry UI meaning.
- Removed unused textured-quad demo sources and robot texture after switching the production target to instanced cubes.
- Snap and bounce originally shared a 180ms progress value. A failing 200ms landing check caught the shorter bounce; snap now uses its own 180ms fraction and visual progress lasts 220ms.
- Build/JVM tests/lint pass with 0 lint errors and 8 warnings (existing dependency versions and portrait policy). Connected testing hit inaccessible emulator storage/lock-screen state, so an independent AVD is being created under /tmp rather than resetting existing profiles.
- Device regression after Activity recreation: Resume was ignored because the recreated focused window did not emit a new focus-gain callback. A Java/native trace confirmed native focus stayed false while the DecorView already had focus. onResume now synchronizes attached-view focus into the installed GameActivity native glue. Native lifecycle callbacks own automatic pause; redundant global pause commands were removed. Resumed/focused flags remain independent, and Android events are drained before UI commands.
- Tall-phone regression: fixed vertical framing clipped the house at the swing extremes. Camera sizing now guarantees at least 3.3 world units of horizontal half-width; a failing aspect-ratio test covers narrow phones and landscape fallback.
- Hardware Back regression: GameActivity's default native filter consumes all keys except volume/camera, bypassing the Android dispatcher. Since Android Views own this game's input, native key/motion filters now reject those events for framework handling. Predictive Back is explicitly enabled; the existing pressBack integration check covers pause/menu behavior.


### Delivery review
- Hard gate PASS: APK built and real controls exercised; settings/record persist; domain and JNI bounds validate inputs. Android-specific checks apply; web navigation/layout checks do not apply.
- Purpose PASS: pastel cube houses follow the approved brief; Android Views provide readable controls; a shared instanced cube avoids per-floor GPU allocation.
- Visual direction PASS: casual toy-city game, energy 2 / rhythm 1 / motion 2; the suspended house is the gameplay focal point, and motion communicates timing, placement and tower progress. Text/button contrast checks passed at 10.93:1 / 7.31:1.
- Craft PASS: domain/scene have no platform includes; host/JVM/instrumented checks pass; narrow-screen clipping and recreation/Back regressions were reproduced and corrected. Hardware-performance limits are explicitly recorded above.
