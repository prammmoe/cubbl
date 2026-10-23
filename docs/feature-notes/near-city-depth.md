# Near-city depth

## 2026-10-23

Strengthens the city row directly in front of the tall skyline: alternating facade values, projecting corner strips, a darker foundation/plinth and roof-underhang shadow. Replaces paired window sills with continuous projecting floor ledges and recessed shadow strips. Uses the existing cube mesh, lighting and fog; no new assets or dependencies.

Camera, time-of-day presets, gameplay, collision and UI stay unchanged. The reference city at half-width 6 grows from 1,211 to 1,279 parts, below the existing 1,500-part test ceiling. No Git branch is available in this workspace.

Verification:

- CMake build and CTest: all three native suites passed, including procedural validity/determinism/part-count checks and physics/gameplay regressions.
- `:app:assembleDebug :app:testDebugUnitTest :app:lintDebug :app:assembleDebugAndroidTest` passed.
- Pixel 8 emulator (API 37, 1080×2400): all six instrumentation tests passed in 57.751 seconds.
- Inspected [gameplay](screenshots/near-city-depth/game.png) and [raised-camera gameplay](screenshots/near-city-depth/canal.png): projecting floor edges, dark recesses and corner strips visibly separate the front city row from the softer skyline. No camera zoom changes.
- Physical-device 60 FPS remains unmeasured. APK: `app/build/outputs/apk/debug/app-debug.apk`.


## 2026-10-23 — Corrected to fog-only depth

The user clarified that depth means fog in front of the buildings. Reverted the added facade contrast, corner strips, plinth, cornices and shadow geometry to the previous city assets (1,211 reference parts). Added a smooth near-city haze term to the existing fragment shader, stronger near the street and fading upward. Reuses the existing atmosphere palette and distant fog, with no extra mesh or draw pass. Gameplay, camera and UI remain unchanged.

Correction verification:
- CMake build and all three CTest suites passed; reference city count restored to 1,211 parts.
- Debug APK and instrumentation APK builds passed.
- All six emulator instrumentation tests passed (50.366 seconds), including graphics initialization and gameplay/lifecycle regressions.
- Inspected fresh [ground-level](screenshots/near-city-fog/game.png) and [raised-camera](screenshots/near-city-fog/canal.png) screenshots: original facades restored, soft near-city fog visible, tower and HUD clear. Fog stays in city coordinates as the camera rises.
- Physical-device performance remains unmeasured.


## 2026-10-23 — Denser foreground fog

Increased near-city fog opacity from 0.38 to 0.62 and its upper-height weight from 0.40 to 0.55. The same soft height/depth gradients and time-of-day colors apply; tower, HUD, camera and original building geometry are unchanged.

Verification: debug APK and instrumentation APK builds passed. Both selected emulator tests (menu/play/pause/settings lifecycle and raised-camera canal regression) passed in 23.556 seconds. Inspected the fresh [gameplay screenshot](screenshots/denser-city-fog/game.png); the stronger fog visibly veils the front city row while the playable house and HUD stay clear. [Raised-camera screenshot](screenshots/denser-city-fog/canal.png) also saved.
