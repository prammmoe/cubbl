# Optimized release APK

## 2026-10-23

Enabled AGP 9.3 R8 code/resource optimization for release and selected CMake Release compilation. Existing SDKs, dependencies and ABIs are preserved. Produces a universal APK, not an AAB.

Configured a dedicated local RSA-3072 release signing key. `signing/` is ignored by Git, directory permissions are 0700, key/credentials are 0600. Back up that folder privately for future updates; passwords are not in source code or these notes. Debug signing is unchanged. Release builds without the local signing properties remain unsigned until a signing configuration is provided.

Reference: https://developer.android.com/topic/performance/app-optimization/enable-app-optimization . For AGP 9.3, `optimization.enable = true` includes both code and resource optimization and standard Android keep rules.

Build/artifact verification:
- `:app:assembleRelease :app:lintRelease :app:testDebugUnitTest` passed. This AGP project does not expose `testReleaseUnitTest`; the two JVM tests were run via the existing debug test task.
- All three native CTest suites passed in a separate CMake Release build.
- R8 minification, resource shrinking and resource optimization tasks executed; native compile flags include `-O3 -DNDEBUG` for all four existing ABIs.
- Signed universal APK: `app/build/outputs/apk/release/app-release.apk`, 3,953,506 bytes (3.77 MiB). ABIs: arm64-v8a, armeabi-v7a, x86, x86_64.
- `apksigner verify --verbose --print-certs` passed with APK Signature Scheme v2. Signer certificate SHA-256: `e8b7a80e290f71919be3fa7defa52f823ba3aa9cb2eade0415db9c9cc6be288f`.
- `zipalign -c -P 16 4` passed; manifest debuggable is false. Default R8 rules preserve MainActivity and native method bindings; no broad custom keep rule was added.
- APK SHA-256: `f12b78fc34bb302c8c100bcf6131ca3c1152e115f2f03597aab023c49baa9fd2`.
- Release lint: zero errors and nine existing SDK/dependency/orientation warnings. This workspace has no Git branch.

Runtime smoke test passed on the signed optimized APK (Pixel 9 Pro XL / Android API 35, arm64-v8a, temporary userdata). Verified home, drop, pause, resume, restart, menu and Sound/Vibration settings. Inspected the gameplay screenshot; no JNI or shader errors were logged. The secondary emulator was closed after testing. Screenshots: [home](screenshots/optimized-release/home.png), [gameplay](screenshots/optimized-release/game.png), [pause](screenshots/optimized-release/pause.png), [settings](screenshots/optimized-release/settings.png). An initial install was safely rejected because the AVD's existing Cubbl package has the debug signature. No existing app was uninstalled. The emulator's configured absolute userdata path overrode `-datadir`; testing was redirected with explicit temporary `-data`/`-cache` paths and a path assertion. The primary debug emulator remains untouched. The API 35 system image initializes data from its data directory rather than a separate userdata.img. The subsequent release launch succeeded, but the first-run Android “Viewing full screen” tutorial covered the Start button; the smoke script now confirms that tutorial and records immersive-mode confirmation before continuing. No app crash was logged.

This new release signing identity cannot update an existing debug-signed installation in place. Keep the local signing folder for future release updates. Physical-phone performance remains unmeasured.
