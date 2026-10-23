# Spec: procedural-scene

## Objective
Procedural pastel building geometry. Audience: casual players using one hand on portrait Android 11+ phones. Offline only.

## Stack and structure
Building.h/.cpp; host CoreTests. Existing GameActivity/GLES3/CMake stack; no new production dependencies.

## Commands
- Android build: `./gradlew :app:assembleDebug`
- JVM and lint: `./gradlew :app:testDebugUnitTest :app:lintDebug`
- Native: `cmake -S app/src/test/cpp -B /tmp/cubbl-core-tests -DCMAKE_BUILD_TYPE=Debug && cmake --build /tmp/cubbl-core-tests && ctest --test-dir /tmp/cubbl-core-tests --output-on-failure`
- Android integration: `./gradlew :app:connectedDebugAndroidTest`

## Code style
Plain cube parts and deterministic integer seed. No rendering API calls or per-house GPU allocations.
Example boundary: `PlacementGrade classifyPlacement(float offset, float width);`
Use descriptive names, small functions, RAII and explicit state transitions.

## Testing and success criteria
Three variants with fixed 2.4 x 1.3 x 1.4 body; all part transforms finite and positive scales.
Host tests prove domain/time/geometry. Android tests prove navigation and persistence. Runtime visual checks and physical-device performance measurements are recorded separately.

## Boundaries
- Always: keep dependency direction inward, validate JNI and persisted values, run relevant checks.
- Ask before: changing engine, adding production dependencies, monetization or online services.
- Never: claim unmeasured performance, hide failing checks, add speculative frameworks, store secrets.

## Open questions
None blocking implementation. Physical-device model is selected from devices actually available; emulator results do not establish physical 60 FPS.

City.h generates deterministic colored city CubeParts with window/roof/shop/tree/street details and time-based traffic. Geometry remains finite, positively scaled and bounded by viewport width; it does not participate in physics.

Platform.h supplies cached world-fixed construction-site geometry: 256×24 packed-earth ground at y=-0.15, a 3.1×2 concrete foundation ending at y=0, paving joints and yellow markings, tire tracks, gravel, corrugated fencing around a 13×6.4 site with a gate, cones, pallets and bricks. No floating underside. Above-plane details stay outside the central 2.4×1.4 landing footprint. Fence and props are visual; ground and foundation colliders live in game-core. Geometry uses the existing cube mesh; city scenery remains separate.

Building templates also vary roof palettes by seed and include side windows plus striped shop awnings; decorations preserve the body collision size and shared body pivot.

City scenery includes a deep teal canal below the street quay, with stone blocks, animated ripples and moving boats. The canal fills the lower city backdrop when the camera rises above the construction terrain; all parts stay behind gameplay (z < -3), and retain bounded part counts.

Atmosphere.h defines seeded per-run time-of-day selection and four color palettes. Rendering varies sky gradients, sun/moon position, cloud tint, world lighting and distance haze; night adds procedural stars and bright city lights. City window geometry stays identical across presets, with more warm lit windows at night. No downloaded art, new dependency, wind or gameplay force.

## Skyline depth (2026-10-23)
Distant facades use cooler, desaturated colors with deliberate neighboring light/dark separation, two recessed vertical glass strips and roof-edge trim. Midground facades are moderately muted; nearby facades retain richer colors. Column/layer color selection is independent of viewport width. Existing city part-count bounds remain unchanged.

## Near-city fog (2026-10-23)
Separate the nearest city row from the tall skyline with a soft atmospheric veil, strongest at street level and fading toward the roofs. Apply it in the existing city fragment shader using world depth and city-relative height, retaining the time-of-day fog palette and distant haze. Near-city opacity is 0.62, with an upper-height weight of 0.55 to retain a visible veil toward the roofs. Keep original building geometry, colors and window sills; do not add architectural detail for this effect. Gameplay, UI and camera remain clear and unchanged.
