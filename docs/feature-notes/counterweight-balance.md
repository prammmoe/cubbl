# Counterweight Balance Implementation Notes

Branch: unavailable (workspace has no Git repository).

## 2026-10-23 — Load-aware tower sway

### Plan
1. Add regression scenarios for modest counterweight, continued lean, excessive opposite overhang and mirror symmetry.
2. Replace adjacent-position-only imbalance with the center of mass above every support, retaining the worst-joint safety check and relative-angle risk. Keep the existing Box2D motion and collapse rules.
3. Run native physics/session tests, centered 50-drop stability and Android build/UI checks.

### Changed
- `towerImbalance` now accumulates the mass center above each support, measuring the horizontal gravity lever arm relative to its rotated top-center. Equal collider masses allow a reverse running mean in O(n) time with O(1) extra storage.
- Kept the worst-joint/relative-angle guard, so an overall centered tower cannot hide a dangerous overhang. Foundation width is shared between physics and the balance calculation.
- Preserved the Perfect-placement zero-force zone. Initial COM-only feedback amplified solver drift during centered stacking; the unchanged 50-drop test caught a failure on drop 49. Applying the existing all-joints-Perfect guard prevents that artificial excitation. No physics/collapse/test thresholds were relaxed.
- Added modest counterweight versus continued-lean, opposite-side symmetry, dangerous globally centered overhang and empty-stack tests. A physical four-drop counterweight sequence remains supported through five seconds of driven sway and retains its real offset.
- Existing gravity, materials, scoring, crane speed, physics motion, pause/restart, collapse thresholds and JNI contracts are unchanged.

### Verification
- Red check: the old metric failed `counterweight < leaning` before implementation.
- CTest: all three suites passed, including 50 centered drops plus 10 seconds settling, height-dependent sway, collapse/miss priority, restart, pause and 30/60/120 FPS equivalence.
- AddressSanitizer + UndefinedBehaviorSanitizer: all three suites passed.
- `:app:verifyPaparazziDebug :app:assembleDebug :app:assembleDebugAndroidTest :app:assembleRelease :app:lintDebug`: passed; native libraries rebuilt for all four existing Android ABIs. UI goldens remain unchanged.
- Full Android instrumentation suite passed on a temporary Pixel 8 / Android 37 emulator with host GPU: 7 tests, 55.674 seconds. Includes physical landing/collapse, lifecycle/recreation, input/navigation, preferences, audio loading and weather/canal regressions. Temporary emulator removed afterward.
- No physical-device listening/performance measurement was performed; 60 FPS on a physical phone remains unverified.
