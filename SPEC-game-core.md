# Spec: game-core

## Objective
Deterministic stacking rules. Audience: casual players using one hand on portrait Android 11+ phones. Offline only.

## Stack and structure
Game.h/.cpp; tests in app/src/test/cpp. Existing GameActivity/GLES3/CMake stack; Box2D 3.1.1 vendored locally, hidden behind PhysicsWorld.

## Commands
- Android build: `./gradlew :app:assembleDebug`
- JVM and lint: `./gradlew :app:testDebugUnitTest :app:lintDebug`
- Native: `cmake -S app/src/test/cpp -B /tmp/cubbl-core-tests -DCMAKE_BUILD_TYPE=Debug && cmake --build /tmp/cubbl-core-tests && ctest --test-dir /tmp/cubbl-core-tests --output-on-failure`
- Android integration: `./gradlew :app:connectedDebugAndroidTest`

## Code style
Pure C++17; no JNI, Android, OpenGL, audio or storage imports. Game owns mutable session state.
Example boundary: `PlacementGrade classifyPlacement(float offset, float width);`
Use descriptive names, small functions, RAII and explicit state transitions.

## Testing and success criteria
Approved physics contract: tasks/plan.md. Dynamic 2D houses, support graph and .35s landing, local offset/angle grading, .2s collapse debounce, 1.5s collapse phase. Scores 100/60/30, capped Perfect combo, three lives, speed cap 3x. 50 centered houses remain upright. Pause preserves world and timers; restart creates a new world.
Host tests prove domain/time/geometry. Android tests prove navigation and persistence. Runtime visual checks and physical-device performance measurements are recorded separately.

## Boundaries
- Always: keep dependency direction inward, validate JNI and persisted values, run relevant checks.
- Ask before: changing engine, adding production dependencies, monetization or online services.
- Never: claim unmeasured performance, hide failing checks, add speculative frameworks, store secrets.

## Open questions
None blocking implementation. Physical-device model is selected from devices actually available; emulator results do not establish physical 60 FPS.

## Physics tuning (2026-10-23)
Contact damping ratio 2, contact hertz 30 (upstream default), sleep threshold 1. All colliders stay dynamic and wake on engine contacts. Gravity, shape size, friction, restitution, four substeps and collapse thresholds are unchanged. Fifty exact center-line drops at 1.5s intervals plus 10s settling must retain foundation support and remain within 3° without collapse. This exact-position physics test complements Game integration tests driven through Start/Release.

## Height and placement difficulty (2026-10-23)
Crane speed becomes min(3, 1 + .04 × successful placements), increasing every accepted floor (period 3s initially, 1s at 50). Failed attempts do not advance speed; restart restores 1×.
Tower sway is deterministic horizontal acceleration applied to accepted dynamic bodies before each physics step. Placement imbalance uses the worst horizontal lever arm of the combined center of mass above each supporting floor, or the relative angle at that joint; the Perfect tolerance (5% width/3°) is a zero-force dead zone. Error saturates at 30% width/30°. Sway begins above two floors, increases .10 m/s² per additional floor, caps at 3 m/s², and is stronger near the top. Period slowly shortens with height. No random wind or visual pose correction.
Sway stops during pause/menu/game over and is no longer driven during Collapsing; existing collapse detection and scoring rules remain. Centered 50-house stability, frame-rate equivalence, and lifecycle checks remain required.

## Construction ground (2026-10-23)
A separate static ground collider spans 256 units at y=-0.15; foundation spans 3.1 units at y=0 with a 0.15-unit thickness. PhysicsWorld::onGround reports direct supporting terrain contact (vertical normal ≥0.5). Ground never contributes to the foundation support graph or spawn selection. An active house touching ground enters the existing 800ms miss recovery once, even if also touching its target; falling-below-target remains a fallback. Collapse evaluation precedes landing/miss. Failed houses remain physical during recovery and are removed before the next spawn.

## Counterweight-aware sway (2026-10-23)
All houses have the same collider and density. For each accepted floor, accumulate the mean world X of that floor and every accepted floor above it. Measure its horizontal gravity lever arm relative to the top-center of the supporting body (foundation for the first floor), using the support top face's projected width. Keep the 5% width dead zone, 30% width saturation and 3°/30° relative-angle risk; use the worst joint. Traverse top-to-bottom in O(n), O(1) extra storage. Preserve the existing zero-force guard when every joint remains within Perfect offset/angle tolerances, preventing tiny solver drift from generating artificial sway in centered towers.
A modest opposite-side placement must reduce a leaning stack's imbalance versus its prior state and versus extending the lean. A large opposite overhang must remain dangerous even when the overall center of mass is centered. Mirrored stacks behave equally. Counterweight changes only the artificial sway amplitude; no pose correction or artificial upright torque. Box2D handles the actual new load/impact. Active released houses affect bodies through contacts, joining the sway calculation after accepted landing. Existing height/speed, 45°/200ms collapse, support-loss detection, pause/restart, 50 centered-drop stability and frame-rate equivalence remain unchanged.
