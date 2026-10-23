# Baloo 2 and Pastel UI Implementation Notes

Branch: unavailable (workspace has no Git repository).

## 2026-10-23 — Font and reference palette

### Changed
- Bundled variable Baloo 2 (400–800) offline. Overlay text and buttons use weighted faces; the app and native Settings theme use the same font.
- Sage/Ivory containers replace navy/neon panels. Cards have rounded flat faces and dark offset lower edges. Primary buttons use Muted Terracotta, with pressed and keyboard-focus states preserved.
- Primary button labels use 20sp bold for readability (provided dark/terracotta pair has 3.70:1 contrast, suitable for large bold text).
- Removed the night-only font override so Android night mode uses the same font and palette.
- Updated icon snapshot colors to use the production color resources; scene snapshots cover home, tutorial, gameplay, pause, collapse and both game-over reasons.

### Font provenance
- Official font: https://raw.githubusercontent.com/google/fonts/main/ofl/baloo2/Baloo2%5Bwght%5D.ttf
- Metadata: https://github.com/google/fonts/blob/main/ofl/baloo2/METADATA.pb
- Copyright 2019 The Baloo 2 Project Authors; SIL Open Font License 1.1, shipped at app/src/main/assets/licenses/Baloo2-OFL.txt.

### Stayed The Same
- Native scenery, weather, physics, scoring, audio, storage and single-Start home navigation.
- No production dependency or runtime font download added.

### Verification
- Reviewed all seven full-scene Paparazzi images and the icon strip; Baloo 2 and the supplied palette render correctly.
- `:app:verifyPaparazziDebug`: 10 JVM tests, zero failures/errors (eight screenshot tests plus two existing JVM checks).
- `:app:assembleDebug :app:assembleDebugAndroidTest :app:assembleRelease :app:lintDebug`: passed; lint has no errors and ten existing warnings (dependency updates, orientation/API usage and programmatic View constructor).
- Android 37 / Pixel 8 temporary emulator, host GPU: `GameFlowTest#menuPlayPauseRestartAndSettings` passed in 19.169 seconds. Includes playing, pause/resume, restart/menu, setting changes, lifecycle/recreation, and persistence. Final button-size adjustment is covered by refreshed Paparazzi snapshots.
- Visually checked the emulator Settings screenshot: labels, title and Done all use Baloo 2 with ivory/dark colors. Retained Settings/gameplay screenshots in docs/screenshots/baloo-pastel-ui. Temporary emulator removed after testing.
- Optimized release APK contains the 683,200-byte bundled font and SIL OFL license. No physical-device performance measurement performed for this UI change.

## 2026-10-23 — Pressed button edge

### Bugs And Fixes
- Bug: moving the colored face down exposed the full-height dark backing above it, making the bottom edge appear to jump to the top.
- Fix: use a transparent backing color while pressed. StateListDrawable resets child alpha during state changes, so setting child alpha to zero alone did not hide the edge; the regression test caught this. The face still moves down; releasing or cancelling the pressed state restores the bottom edge. The shared drawable covers text buttons and Pause/Settings icons; keyboard focus retains its separate outline.

### Verification
- Added a pressed Start scene and a pixel regression check for the hidden upper edge, filled bottom and restored lower edge after release.
- `:app:recordPaparazziDebug` and `:app:verifyPaparazziDebug`: passed, 12 total JVM tests including the pressed scene and press/release pixel regression. Visually checked that the pressed Start has no dark upper edge.
- `:app:assembleDebug :app:assembleRelease :app:lintDebug`: passed.

## 2026-10-23 — Larger HUD and vibrant hearts

### Changed
- Increased Lives/Score labels 10→13sp, score maximum 32→42sp, Best maximum 11→15sp and Streak 14→16sp. Kept auto-sizing for large numeric scores.
- Expanded score-card rows and height to fit the text; moved feedback below the taller HUD.
- Changed active hearts to saturated cherry red #C72E48 with a dedicated color resource, increased icons to 22×24dp, and widened the lives card to 92dp. Lost hearts remain dark outlines; other accent colors are unchanged.

### Verification
- `:app:recordPaparazziDebug` and `:app:verifyPaparazziDebug`: passed; 12 JVM tests, zero failures/errors. Visually checked larger gameplay typography, vibrant hearts and feedback spacing during collapse.
- `:app:assembleDebug :app:assembleRelease :app:lintDebug`: passed. Existing button press/release regression remains green.
- No new emulator run for this visual-only change; snapshots use the shared production overlay.

## 2026-10-23 — Ivory Settings button

### Changed
- Home Settings button now uses the menu panel's ivory fill; the symbol stays dark. Existing lower edge, pressed/focus states and 48dp target are preserved. Pause stays sage.

### Verification
- Reviewed the home snapshot: Settings background exactly matches the ivory menu panel and the symbol remains dark.
- `:app:recordPaparazziDebug :app:verifyPaparazziDebug`: passed (14 JVM tests, including press/release regression).
- `:app:assembleDebug :app:assembleRelease :app:lintDebug`: passed.
