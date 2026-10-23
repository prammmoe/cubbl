# Detailed platform assets

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Replaced the flat landing slab with a cached procedural floating foundation: inset stone paving, visible grout, copper center inlay/trim, masonry faces, stepped stone supports, corner lanterns and terracotta flower pots.
- Replaced the city ground's plain green strip with layered slate embankment, masonry facing and sandstone pavement. Green remains in vegetation.
- House assets now use five coordinated roof colors, side windows and striped shop awnings. Platform visibility follows viewport height so narrow/tall screens retain the foundation while it is visible.
- Uses existing cube instancing and local procedural parts; no assets downloaded or dependencies added.

### Stayed the same
- Foundation landing width 3.1 and surface y=0; physics, house collision dimensions, scoring and controls.
- Animated city background and shared GPU mesh/instancing.

### Verification
- Native checks validate geometry/color bounds, positive scales, and a clear central landing area with no decorative part above the collision plane.
- Three host CTest executables pass, including platform clearance checks and deterministic varied roof palettes.
- `./gradlew :app:assembleDebug :app:testDebugUnitTest :app:lintDebug` passed for all four native ABIs.
- AndroidJUnitRunner: four tests passed on Pixel_8 AVD (Android 17/API37, 1080×2400), covering gameplay, pause/recreation, collapse, restart and settings.
- Reviewed [new foundation/house assets](screenshots/detailed-platform/playing.png) and [collapse](screenshots/detailed-platform/collapse.png) screenshots; physics collider geometry is unchanged.
- Physical-device performance remains unmeasured.
