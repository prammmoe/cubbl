# City Launch Screen

Branch: `main`

## 2026-10-24 - Replace the white launch screen

### Changed
- Set an explicit warm sunset color for the Android 12+ system splash and retained the current city/house launcher icon.
- Reused the generated sunset city artwork, cropped for portrait screens, as the launch window background and a temporary full-screen city cover over the native surface.
- Added a thread-safe native first-frame signal. The cover leaves after the first successful EGL swap or a reported renderer failure, without a minimum display timer.
- Skip the system icon exit fade so it does not linger over the city/game transition. Android 11 uses the city window background directly.

### Verification
- Reviewed the supplied recording and reproduced the missing sunset theme attribute with `LaunchAppearanceTest` before the fix.
- `LaunchAppearanceTest` passed on the Android 17 emulator after the fix, including first launch, renderer readiness and Activity recreation.
- `GameFlowTest#menuPlayPauseRestartAndSettings` passed on the same emulator; build and `git diff --check` passed.
- Android 11 uses the resource fallback but was not tested on a separate device. The optional cold-start video capture did not complete because its launch command permission was superseded by the later test request.

### Platform constraint
- Android 12+ permits only a solid color for the system splash background. The system stage uses sunset peach; the full-screen city appears in the Activity loading stage. See [Android splash screen documentation](https://developer.android.com/develop/ui/views/launch/splash-screen).
