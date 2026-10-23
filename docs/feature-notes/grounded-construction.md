# Fixed Camera and Grounded Construction

Branch: none (workspace is not a Git repository).

## 2026-10-23 — Implementation

### Changed
- Zoom now depends only on aspect ratio; camera position still follows tower X/Y. Pause/collapse freeze position, and surface resize can update scale.
- Replaced floating masonry base with a low concrete foundation on packed earth, tile joints, yellow markings, gravel, tire tracks, corrugated construction fencing, a gate, cones, pallets and bricks. Geometry stays cached and world-fixed.
- Added a separate static ground collider and PhysicsWorld::onGround. Ground contacts do not establish foundation support. Active ground contact triggers the existing miss recovery once.

### Stayed The Same
- Foundation landing top y=0, house dimensions, 350ms landing acceptance, 800ms miss recovery, collapse priority, score/preferences and JNI snapshot.
- No new dependency or downloaded asset.

### Bugs And Fixes
- Dynamic horizontal reach and active-height fitting caused zoom changes. Removed those scale adjustments at the camera source.
- The old camera-relative skyline fell below solid terrain on narrow screens. Clamped its street level to the terrain surface so city facades and traffic remain visible.
- A thin ground slab exposed sky below its foreground edge in screenshots. Extended its earth volume below the portrait viewport and softened dirt patches.
- The old first-house collapse fixture now rests on solid ground; replaced it with a physically offset tower. Kept the same collapse thresholds and 50-house stability requirement.

### Verification
- CTest: all three executables pass, including fixed zoom, terrain contact isolation, ground miss/pause/recovery, deterministic frame patterns and centered 50-house stability.
- Android assembleDebug (all four existing ABIs), two JVM tests and lint passed (0 errors, 9 existing warnings). Final APK instrumentation passed all four tests; the previous large-offset fixture was replaced with gradual offsets because terrain now catches edge drops.
- ASan/UBSan: all three host executables passed.
- Pixel_8 emulator: full four-test instrumentation passed after the final render change; menu/play/pause/recreation/settings also passed separately at 800×2400. Original 1080×2400 resolution restored. Inspected playing and collapse screenshots plus narrow portrait; terrain has no exposed underside, and the city remains visible.
- Screenshots: [playing](screenshots/grounded-construction/playing.png), [collapse](screenshots/grounded-construction/collapse.png), [narrow portrait](screenshots/grounded-construction/playing-narrow.png).
- Physical-device 60 FPS remains unverified; no physical phone available.
