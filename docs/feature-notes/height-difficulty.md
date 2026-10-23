# Height-dependent sway and crane speed

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Crane speed now rises every accepted placement: `min(3, 1 + .04 × placements)`. Initial period 3 seconds, 1.5 seconds at 25 placements (2×), 1 second from 50 accepted placements onward. Misses do not advance difficulty.
- Tower imbalance measures adjacent accepted house centers along the local support plane and their shortest relative angles. Perfect alignment tolerance (5% width, 3°) produces no added force; error saturates at 30% width or 30°.
- Sway drives real Box2D horizontal forces, proportional to body mass. It starts above two floors, grows .10 m/s² per extra floor (maximum 3), and scales toward the top. A deterministic sinusoidal phase replaces any need for random wind or renderer-only pose changes.
- The phase freezes on pause and resets on restart. Collapsing runs without added driving forces; original collapse rules determine the outcome.

### Stayed the same
- Collider sizes, gravity, four physics substeps, support/landing timer, scoring, lives, input, JNI snapshot, renderer assets and persistence.

### Verification
- Three host CTest executables pass. New tests cover speed per floor/cap, perfect dead zone, positional/angular errors, height scaling, upper-floor scaling and force application.
- Physical sway comparison: alternating ±.24m stacks at 8 floors deviated about .023m from an undriven control; 16 floors about .250m over ten simulated seconds. Regression asserts a clear increase, with tolerances rather than exact engine poses.
- Fifty centered drops plus ten seconds settling pass with the new sway policy active during every step, retaining support and angles below 3°.
- A six-floor imperfect Game session is equivalent at 30/60/120 render FPS and reaches collapse. A five-floor imperfect session proves pause freezes the sway phase and restart matches a fresh run.
- Android APK for all four native ABIs, JVM tests and lint pass. AndroidJUnitRunner on Pixel_8 AVD (Android 17/API37): all four integration tests passed.
- AddressSanitizer/UndefinedBehaviorSanitizer host build: all three CTest executables passed with the new force/scaling and pause/restart scenarios.
- Physical-device 60 FPS remains unmeasured.
