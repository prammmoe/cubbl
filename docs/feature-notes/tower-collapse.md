# Tower collapse implementation notes

Branch: none (workspace has no Git repository).

## 2026-10-23 — Dynamic stacking

### Changed
- Vendored unmodified Box2D 3.1.1 and MIT license; archive hash/source in app/src/main/cpp/vendor/README.md. Static PIC build, all four existing Android ABIs; upstream extras are excluded.
- PhysicsWorld owns bodies and foundation contact graph. House IDs and center-based HousePose replace bottom-Y coordinates. Released houses start upright with zero velocity; accepted houses remain dynamic. No snap, artificial bounce, forced alignment or static locking.
- Landing requires 350 ms of target support connected to foundation. Local offset/relative angle determine score; collapse has priority before score/miss. Failed bodies remain physical during 800 ms recovery.
- Collapsing phase 7 runs 1.5 seconds; 200 ms tilt/support-loss debounce; GameOverReason appended at JNI index 8. UI displays collapse and out-of-lives reasons; collapse uses one existing miss sound event.
- Instanced renderer rotates geometry and normals about the body center. Camera tracks actual X/Y and rotated bounds, freezes during collapse, persists across surface/Activity recreation and resets for menu.

### Stayed the same
- Action IDs and phase IDs 0–6, one-tap portrait controls, score values/combo cap, lives, difficulty, procedural templates, offline preferences/high score and lifecycle pause policy.

### Bugs and fixes
- Default Box2D sleeping/contact damping accumulated drift under repeated impacts. Tuned contact damping ratio to 2 and sleep threshold to 1; kept four substeps, gravity 12, friction .8 and restitution 0. All houses remain dynamic, with engine sleeping/wake behavior.
- Exact center-line stability uses 50 drops at X=0, 1.5 simulated seconds per drop, followed by 10 simulated seconds settling. Support/angle-collapse trackers run during every step; each house must remain supported and within 3 degrees. This is separate from Game input tests, whose taps are quantized to simulation steps and whose spawn centers can drift with supports.
- Visual QA found decorative ground occluding bodies falling outside the physical platform. Moved that scenery behind the play area; foreground platform bounds still match the static collider.
- Camera originally followed old tower state after returning to menu; explicit menu framing fixes it. Camera storage is process-lived like Game.
- The old navigation test sometimes injected its Resume tap while the recreated panel was still moving with window insets. Native trace showed no UI handler invocation. Tests now wait for displayed button bounds/window focus to stabilize before tapping; assertions and lifecycle behavior are retained.

### Verification
- Host CTest: core policies/generator/clock, PhysicsWorld including 50-house stability, and Game integration including both endings, offset retention, pause/velocity continuity, collapse event priority, 100 restarts, 30/60/120 FPS equivalence, rotated bounds and narrow framing.
- AddressSanitizer/UndefinedBehaviorSanitizer host build: all three CTest executables pass, including 100 restarts and world destruction.
- Final `./gradlew :app:assembleDebug :app:testDebugUnitTest :app:lintDebug :app:connectedDebugAndroidTest`: successful. APK contains arm64-v8a, armeabi-v7a, x86 and x86_64. Two JVM tests and four instrumentation tests pass; lint has 0 errors/9 retained warnings.
- Device: Pixel_8 AVD, Android 17/API37, sdk_gphone16k_arm64, 1080×2400. Tests cover menu/settings/Back/background, native snapshot length/reason, landing and collapse pause/recreation, restart and ACTION_CANCEL.
- Repeated the complete four-test Android suite directly with AndroidJUnitRunner: all four passed again. The final visual test waits for the collapse HUD to render before capture; screenshots in [tilted.png](screenshots/tower-collapse/tilted.png) and [game-over.png](screenshots/tower-collapse/game-over.png).
- After the scenery correction, rebuilt all four APK ABIs and reran the landing/collapse/recreation visual test successfully. Reviewed screenshots: body geometry/lighting rotate together, held house/crane disappear during collapse, and the game-over panel preserves score and shows the reason.
- Physical-device 60 FPS, audible playback, deliberate EGL context-loss injection and long-run GPU memory profiling remain unverified; emulator checks do not establish physical performance.
