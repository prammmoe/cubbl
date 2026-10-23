# English Arcade Copy

Branch: none (workspace has no Git repository).

## 2026-10-23

### Changed
- Replaced Indonesian menu, HUD, settings, pause, tutorial, placement feedback, game-over and display-error copy with English arcade wording.
- Menu: “Stack high. Stay steady.” and “Start stacking”. Feedback: “Perfect!”, “Nice drop!”, “Close call!”, “Miss!” and “Timber!”. End screen: “Run over!” with a clear reason and “Stack again”.
- Centralized all UI text in Android string resources, including formatted scores/lives and the plain-language game-area accessibility label. Perfect combos display as Streak, not a misleading score multiplier.
- Updated existing instrumentation selectors for the English controls and announcements.

### Stayed The Same
- Gameplay, scoring, preferences, audio/haptic behavior, layout and touch controls.

### Bugs And Fixes
- The old lifecycle test used the same collapse label during animation and on the end screen. It now checks “Timber!” during collapse and “Tower down!” after game over, avoiding a race with the short remaining animation after resume.

### Verification
- Android assembleDebug, two JVM unit tests, lint and assembleDebugAndroidTest passed. Lint: 0 errors, 9 existing warnings.
- AndroidJUnitRunner: all five instrumentation tests passed on the Pixel_8 emulator at 1080×2400, including English menu/settings, pause/recreation, collapse/end screen and elevated-camera gameplay.
- Inspected the [English menu screenshot](screenshots/english-arcade-copy/menu.png): tagline and all buttons fit. Other states were checked through their English labels by instrumentation. Runner cache screenshots were unavailable afterward, so the menu was captured directly.
- Source scan found no remaining Indonesian player-facing strings in production or instrumentation selectors. Copy audit passed: resource-backed English text, clear actions/reasons, and no unsupported score-multiplier claims.
