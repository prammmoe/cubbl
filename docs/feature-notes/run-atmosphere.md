# Per-run Atmosphere

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Start and Restart choose Morning, Day, Sunset or Night randomly, excluding the immediately previous preset after the first run. Seed is initialized once from the monotonic clock; host tests can supply a fixed seed.
- Process-owned RunAtmosphere survives pause, background, renderer rebuild and Activity recreation. Menu keeps the last atmosphere. Domain state and nine-value JNI snapshot are unchanged.
- Four sky palettes, sun/moon positions, cloud colors, scene tint and haze. Night adds crescent moon/stars and brighter city windows, streetlights/headlights.

### Stayed The Same
- Physics, scores, lives, controls, fixed camera zoom, settings/preferences and offline operation. No new dependency or asset download. This feature changes time-of-day scenery rather than introducing rain or wind.

### Verification
- Three host CTest executables passed. Atmosphere checks cover 1,000 seeded selections, reproducibility, no consecutive repeats, all four presets, finite palettes and night/day window differences without geometry changes.
- ASan/UBSan: all three host executables passed.
- Android assembleDebug (four existing ABIs), two JVM tests, lint and assembleDebugAndroidTest passed. Lint: 0 errors, 9 existing warnings.
- All six Android instrumentation tests passed on Pixel_8 AVD at 1080×2400. New checks sample the rendered sky across six Start/Restart runs, verify consecutive colors differ, and confirm pause/resume/recreation retain the same palette. Renderer-error flag remained clear.
- Captured and inspected [morning](screenshots/run-atmosphere/morning.png), [day](screenshots/run-atmosphere/day.png), [sunset](screenshots/run-atmosphere/sunset.png) and [night](screenshots/run-atmosphere/night.png). Foreground and HUD remain readable, with moon/stars and warm city lights visible at night.
- Physical-phone 60 FPS remains unmeasured.
