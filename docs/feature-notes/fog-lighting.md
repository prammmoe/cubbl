# Distance Fog and Lighting

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Replaced flat per-cube haze with per-fragment distance fog and denser low-lying mist; fog colors blend the active horizon and mid-sky palette.
- Added atmosphere-specific directional light/color: warm morning/sunset, high daylight and cool moonlight.
- Warm windows, lamps and headlights gain an emissive accent after sunset, softened with distance. Analytic warm light pools align with existing street lamps on horizontal city surfaces.

### Stayed The Same
- Building geometry, draw passes, textures/dependencies, gameplay, camera, UI and per-run atmosphere selection. Fog affects scenery, preserving foreground tower clarity. No bloom framebuffer or new light model assets.

### Verification
- All three host CTest executables passed, including four-atmosphere lighting direction, fog/color ranges and dusk/night emission strength checks.
- Android assembleDebug, JVM tests, lint and assembleDebugAndroidTest passed; all six emulator instrumentation tests passed with no renderer error.
- Inspected fresh [morning](screenshots/fog-lighting/morning.png), [day](screenshots/fog-lighting/day.png) and [night](screenshots/fog-lighting/night.png) screenshots on Pixel_8 AVD (1080×2400). Distant haze is softer while foreground tower edges and warm night lights remain visible. The random run sample did not refresh the sunset screenshot, so the stale copy was excluded from this feature gallery.
- Physical-device frame-rate measurement remains unavailable.
