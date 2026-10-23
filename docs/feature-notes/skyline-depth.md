# Skyline Depth

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Tall distant buildings no longer share a narrow blue-purple value range: adjacent facade lightness varies deliberately, with cooler far/mid palettes and richer nearest facades. Color selection remains stable across viewport widths.
- Added recessed glass strips to distant towers and thin roof-edge highlights to separate overlapping silhouettes.
- Strengthened background side-face shading and smooth distance haze tied to the current atmosphere; night windows remain bright.

### Stayed The Same
- Gameplay, camera zoom, time-of-day selection, UI, instanced cube renderer and dependency set. Existing city geometry budget remains enforced.

### Verification
- All three host CTest executables passed. Added checks for distant-neighbor luminance separation and paired glass strips in all four atmosphere modes; existing finite geometry and <1500-part viewport budget checks remain enforced.
- Android assembleDebug, JVM tests, lint and assembleDebugAndroidTest passed. All six emulator instrumentation tests passed, including atmosphere switching/recreation, settings and gameplay endings.
- Inspected fresh [morning](screenshots/skyline-depth/morning.png), [day](screenshots/skyline-depth/day.png), [sunset](screenshots/skyline-depth/sunset.png) and [night](screenshots/skyline-depth/night.png) screenshots on Pixel_8 AVD (1080×2400). Distant glass strips, roof rims and darker side faces distinguish the building layers while foreground houses remain legible.
- Physical-device frame-rate measurement remains unavailable.
