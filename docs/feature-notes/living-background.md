# Living city background

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Sunset sky shader: blue/lavender/apricot gradient, sun halo, layered drifting clouds and animated birds; fine dithering reduces gradient banding.
- Three city layers with terracotta, teal, ochre, rose and indigo facades, roof details, lit windows, striped shop awnings, trees, street lamps, road markings and moving cars.
- Background uses the existing instanced cube mesh plus one fullscreen sky draw. Local source only, no new assets, downloads or dependencies.
- Menu redraws while visible for ambient animation; paused/background/game-over screens remain idle and pause freezes the scenery animation.

### Stayed the same
- Physics, collision dimensions, house appearance, scoring, lives, storage and controls.

### Verification
- Host checks cover finite geometry, positive dimensions, color bounds, deterministic layout, bounded part counts and moving cars.
- Three host CTest executables passed, including existing physics/game regressions and new city geometry/animation checks.
- `./gradlew :app:assembleDebug :app:testDebugUnitTest :app:lintDebug` passed for all four native ABIs; no new lint errors.
- AndroidJUnitRunner: all four instrumentation tests passed on Pixel_8 AVD (Android 17/API37, 1080×2400), including pause/recreation and both UI flows.
- Reviewed [gameplay](screenshots/living-background/playing.png) and [menu](screenshots/living-background/menu.png) screenshots: gradient sky, cloud/bird layers, rich facades and unobstructed foreground houses.
- Final scene polish: rebuilt APK and reran the menu/play/pause/restart/settings instrumentation successfully; refreshed both screenshots.
- Raised road markings above the asphalt surface; car positions use positive wrapping so traffic remains visible after long sessions. Host checks include one million seconds of animation time.
- Physical-device 60 FPS remains unmeasured.

## 2026-10-23 — Canal below the city

### Changed
- Filled the exposed orange sky below the camera-relative city with a teal canal, stone quay, animated ripples and two moving boats, using existing cube instancing.
- Canal extends below the portrait viewport and stays behind gameplay objects. Construction-site terrain, physics and fixed camera zoom are unchanged.

### Verification
- Host geometry checks include canal coverage, finite transforms, deterministic layout and bounded animation; existing part-count limit is retained.
- All three CTest executables pass, including a projected overlap check between quay and water to prevent a thin sky-colored gap.
- Android assembleDebug, JVM tests and lint passed; the added elevated-camera instrumentation stacked eight houses successfully on the Pixel_8 emulator at 1080×1500. Original emulator resolution restored after capture.
- Final seam adjustment: raised the water behind the quay and moved initial ripples/boat closer to the bank. Final Android build/JVM/lint and elevated-camera instrumentation passed; inspected the refreshed [canal screenshot](screenshots/living-background/canal.png).
- Screenshot capture uses direct access to the existing thread-safe JNI queue/snapshot; waiting for UI idle delayed centered releases in the initial fixture. No production test hook was added.
