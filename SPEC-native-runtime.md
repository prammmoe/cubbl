# Spec: native-runtime

## Objective
Android lifecycle, rendering and time. Audience: casual players using one hand on portrait Android 11+ phones. Offline only.

## Stack and structure
main.cpp, Renderer.h/.cpp and native-runtime/FrameClock.h. Existing GameActivity/GLES3/CMake stack; vendored Box2D 3.1.1 linked statically through game-core.

## Commands
- Android build: `./gradlew :app:assembleDebug`
- JVM and lint: `./gradlew :app:testDebugUnitTest :app:lintDebug`
- Native: `cmake -S app/src/test/cpp -B /tmp/cubbl-core-tests -DCMAKE_BUILD_TYPE=Debug && cmake --build /tmp/cubbl-core-tests && ctest --test-dir /tmp/cubbl-core-tests --output-on-failure`
- Android integration: `./gradlew :app:connectedDebugAndroidTest`

## Code style
Game is process-lived and only the native game thread mutates it. Mutex-protected bounded commands and immutable HUD snapshots cross JNI.
Example boundary: `PlacementGrade classifyPlacement(float offset, float width);`
Use descriptive names, small functions, RAII and explicit state transitions.

## Testing and success criteria
Fixed 60Hz steps, at most six catch-up updates, blocking inactive loop; surface recreation retains the session and pauses; cube instancing and visible-floor culling.
Host tests prove domain/time/geometry. Android tests prove navigation and persistence. Runtime visual checks and physical-device performance measurements are recorded separately.

## Boundaries
- Always: keep dependency direction inward, validate JNI and persisted values, run relevant checks.
- Ask before: changing engine, adding production dependencies, monetization or online services.
- Never: claim unmeasured performance, hide failing checks, add speculative frameworks, store secrets.

## Open questions
None blocking implementation. Physical-device model is selected from devices actually available; emulator results do not establish physical 60 FPS.

HousePose uses body centers and Z rotation. Transform part positions and normals consistently. Camera follows real X/Y with smoothing, uses rotated bounds, and freezes position on pause/collapse. Orthographic half-height depends only on surface aspect via viewHalfHeight; lateral movement, height and rotation never change zoom. Surface resizing may recalculate scale. JNI snapshot indices 0–7 unchanged; reason None/OutOfLives/Collapse appended at 8.

## Living city background (2026-10-23)
Render a procedural sunset sky and layered city with clouds, birds and moving traffic. One sky pass plus existing cube instancing; no production dependency or downloaded art. Visible menu animates without advancing Game; paused/game-over/background loops remain idle. Foreground tower remains visually distinct.

Construction terrain is world-fixed and culled with its projected depth extent. While near ground level, city street height stays at or above y=-0.15 so the new solid terrain does not hide city facades/traffic on narrow portrait screens.

## Per-run atmosphere (2026-10-23)
Start from Menu and every Restart select Morning, Day, Sunset or Night with no immediate repeat. The first run may select any preset. RunAtmosphere is process-lived, seeded once from the monotonic clock and referenced by every rebuilt Renderer. Pause, resume, menu return, surface recovery and Activity recreation do not choose another preset. JNI snapshot and GameState stay unchanged; atmosphere has no physics effect.

Background cube side lighting has stronger contrast than playable buildings. Atmospheric haze increases smoothly from near to far using world Z and the active time-of-day fog color. Playable houses retain their existing lighting contrast; warm night windows/lamps remain bright. No extra render pass or texture is required.

## Fog and lighting polish (2026-10-23)
Distance/low-height mist is evaluated per fragment for city scenery using the active sky colors. Per-atmosphere directional key lighting gives warm morning/sunset light, overhead daylight and cool moonlight. Warm emissive accents brighten at sunset/night and attenuate through fog. Repeating analytic light pools align with existing street lamps. Effects reuse the cube pass with no extra models, textures or render targets; gameplay objects remain outside city fog.
