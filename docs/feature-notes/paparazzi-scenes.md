# Paparazzi game scenes

## 2026-10-23

Extracted the existing Android overlay construction into `GameOverlay`, shared by MainActivity and Paparazzi. The Activity retains native commands, release debouncing, lifecycle, preferences and audio/haptic side effects. Snapshot state contains only display values; UI layouts/styles are not duplicated in tests.

The game world is GLES/Box2D, which Paparazzi cannot execute. Emulator tests therefore record clean engine frames directly from SurfaceView using PixelCopy, excluding the Android overlay. Paparazzi composes these static test-only backgrounds with the real current UI to cover home, tutorial, gameplay, pause, collapse and both game-over reasons. These screenshots check UI rendering; they do not replace physics, real-time lighting or lifecycle tests. Fixtures are packaged only in host test resources.

Snapshot recording exposed the theme's invalid `android:fontFamily="sans"` alias, which layoutlib tried to open as a font asset and failed before constructing TextViews. Changed it to the Android family `sans-serif`; explicit arcade typefaces remain unchanged. The existing failing scene tests guard this path.

Visual review caught a duplicated HUD in one fixture: hiding the overlay and waiting for a screenshot was not reliably synchronized with SurfaceFlinger. Fixture capture now uses PixelCopy on the actual native SurfaceView instead of a screenshot of the full window.

To refresh native backgrounds deliberately (with a test device connected):

```sh
./gradlew :app:connectedDebugAndroidTest
mkdir -p app/src/test/resources/scenes
for scene in home tutorial tower collapse; do
  adb exec-out run-as com.example.cubbl cat "files/cubbl-world-$scene.png" > "app/src/test/resources/scenes/$scene.png"
done
./gradlew :app:recordPaparazziDebug
./gradlew :app:verifyPaparazziDebug
```

Normal Paparazzi verification uses the stored fixtures and needs no emulator. Choose a device explicitly with `adb -s` when several are connected.

A fresh software-rendered emulator produced timing-dependent failures in the scripted drop tests. Re-running with host GPU acceleration passed all three affected scenarios, including the original strict three-life assertions. Existing test assertions and gameplay thresholds remain unchanged. The old AVD was low on storage; a dedicated temporary AVD supplied clean test data and was deleted after capture.

Verification:
- Initial shared-overlay regression run: all seven Android instrumentation tests passed (74.186 seconds).
- Final PixelCopy fixture run on a clean GPU-accelerated API 37 Pixel 8 AVD: all three selected menu/lifecycle, collapse/recreation and eight-placement camera scenarios passed (41.217 seconds), retaining their original assertions.
- `:app:recordPaparazziDebug` and `:app:verifyPaparazziDebug` passed: ten JVM tests, including all seven scene snapshots and the prior icon snapshot.
- `:app:assembleRelease` and `:app:lintDebug` passed.
- Inspected native-frame fixtures and the composed goldens. The tutorial now has only one HUD; all four backgrounds are copied from the GL surface without Android UI.
- Report: `app/build/reports/paparazzi/debug/index.html`. Goldens: `app/src/test/snapshots/images/com.example.cubbl_GameSceneSnapshotTest_*.png`.
- The temporary capture AVD was removed. No user app or persisted game data was cleared. Physical-device performance remains unmeasured.
