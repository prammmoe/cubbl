# Minimal Home and Settings

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Home keeps one primary Start button. Sound/Vibration controls moved to a standard Settings dialog opened by an accessible 48dp top-right configuration icon.
- Settings persist immediately; muting stops audio. Done/Back dismiss the dialog. Open dialog state survives Activity recreation; the old window is dismissed during destruction.
- Updated UI tests for settings visibility, both checkboxes, recreation, Done/Back and persistence. Test screenshots now use filesDir rather than ephemeral cache.

### Stayed The Same
- English copy, gameplay, controls, record storage and preference keys. No new dependency.

### Bugs And Fixes
- Opening the restored dialog inside onCreate could leave the recreated main window without focus. Restore is now deferred until the Activity regains window focus.

### Verification
- Android assembleDebug, two JVM tests, lint and assembleDebugAndroidTest passed. Lint: 0 errors, 9 existing warnings.
- All five Android instrumentation tests passed on the Pixel_8 emulator at 1080×2400. Settings checks cover both checkbox values, immediate persistence, activity recreation, Done/Back dismissal and hiding settings controls from home.
- Inspected [home](screenshots/minimal-home-settings/menu.png) and [settings](screenshots/minimal-home-settings/settings.png). Home has one Start CTA, a separate safe-area configuration icon, and readable native settings controls.
