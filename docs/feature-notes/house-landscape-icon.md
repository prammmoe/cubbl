# House Landscape Icon

Branch: `main`

## 2026-10-24 - Replace the default launcher artwork

### Changed
- Generated a single cream box house with a flat terracotta roof, teal windows and door, lawn and blocky trees. The user's gameplay screenshot superseded the initial pitched-roof concept.
- Saved the transparent source, 1024 × 1024 master and circle, rounded-square and themed previews in `assets/icon/`.
- Replaced Android template launcher assets with a transparent 432 × 432 foreground, solid pastel sky background and separate vector house silhouette for themed icons.
- Centered the foreground within 60 × 60 dp of the 108 dp adaptive canvas. Replaced all ten mdpi–xxxhdpi fallback icons with PNG versions.

### Stayed The Same
- Manifest icon resource names, application identity, gameplay and APIs.
- Existing unrelated working-tree changes were preserved.

### Verification
- Visually inspected circle and rounded-square mask previews, the themed silhouette and the 48px fallback icon. The complete house and lawn remain visible.
- Confirmed master and adaptive foreground dimensions, parsed changed XML and checked all density variants.
- `./gradlew :app:assembleDebug --offline` succeeded; Gradle required access to its existing cache outside the sandbox.
- `git diff --check` passed.
- Previews simulate icon masking; no live launcher/device check was performed.

### Reference
- Adaptive layers and safe-area sizing follow [Android adaptive icon guidance](https://developer.android.com/develop/ui/compose/system/icon_design_adaptive).


## 2026-10-24 - City skyline at sunset

### Changed
- Replaced the plain blue background with a geometric pastel city skyline, then changed its sky to peach, golden orange and dusty lavender sunset following the user's refinement.
- Saved both city background sources in `assets/icon/`; the sunset source is used by the adaptive background, master, ten fallback density icons and mask previews.
- Removed the plain-color background XML to prevent duplicate drawable names after adding the 432px bitmap background.

### Verification
- Inspected composed circle/rounded-square previews and the 48px icon for foreground readability and clipping.
- `./gradlew :app:assembleDebug --offline` passed with the sunset background; `git diff --check` passed.


## 2026-10-24 - Front-facing, grounded house

### Changed
- Regenerated the scene with a symmetrical front-facing house and continuous ground shared with the city; removed the floating lawn slab.
- Kept the sunset skyline and blocky trees, and updated the master, all ten launcher density assets and mask previews.
- Rendered the unified scene in the adaptive background so house, ground and skyline move together. Added edge bleed outside the central 72dp viewport; a transparent foreground satisfies the existing adaptive layer reference.
- Updated the themed silhouette to a front-facing house. The earlier transparent house and background sources are retained as design history and are no longer used by the launcher.

### Verification
- Inspected circle, rounded-square, 48px and front-facing monochrome previews. House meets continuous ground and remains visible within the icon masks.
- Confirmed 1024px master, 648px adaptive scene bitmap, valid XML and all ten launcher variants without duplicate foreground resources.
- `./gradlew :app:assembleDebug --offline` and `git diff --check` passed. No live launcher/device check was performed.
