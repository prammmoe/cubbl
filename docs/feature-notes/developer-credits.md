# Developer Credits

Branch: `main`

## 2026-10-24 - Developer page and asset notices

### Changed
- Added Credits from the home menu, naming Prammmoe as Developer and linking to https://prammmoe.space/ through an Android URL span.
- Credits uses the existing pastel panel, font and scroll container. Main menu button/system Back return to home; the page survives Activity recreation.
- Added an Asset licenses dialog containing the bundled Baloo 2, Box2D and audio notices. Bundled the previously missing Box2D MIT notice and retained audio provenance inside the APK.
- Recorded asset provenance and unresolved publication checks in docs/asset-license-audit.md.

### Verification
- CreditsFlowTest reproduced the missing Credits button before implementation, then passed for navigation, developer name, HTTPS link span and Activity recreation.
- LaunchAppearanceTest passed with the added menu entry.
- Asset-license dialog content, menu return buttons and URL span validated by the final CreditsFlowTest; LaunchAppearanceTest passed again.
- Reviewed and refreshed home/home-pressed snapshots and added a Credits snapshot; full `verifyPaparazziDebug` passed.
- `git diff --check` passed. Asset license files confirmed inside the debug APK.
- `:app:assembleRelease --offline` passed, including release lint/R8. All four asset license/provenance notices verified in the release APK.

## 2026-10-24 - Move Credits into Settings

### Changed
- Removed Credits from the home panel and added it beside Done in the Settings dialog.
- Credits now returns to Settings using its Settings button or system Back. The developer, website and license viewer are preserved, including page restoration after Activity recreation.

### Verification
- CreditsFlowTest passed on the Android 17 emulator: Credits absent from home, opens from Settings, preserves the developer/link/license content, survives recreation and returns to Settings through both Back and the page button.
- Reviewed the updated home and Credits snapshots; `verifyPaparazziDebug` and `git diff --check` passed.

## 2026-10-24 - Back label on Credits
- Renamed the Credits return button from Settings to Back; it still returns to Settings.
- Updated the existing navigation test selector and Credits snapshot. Focused Paparazzi recording/compilation and `git diff --check` passed; visually verified the Back label. Emulator navigation was not rerun for this label-only change.
