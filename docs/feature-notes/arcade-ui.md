# Arcade UI

Replaces the cream score/lives cards with a compact navy HUD: pink heart icons, a prominent gold score, a smaller Best label and an accessible Pause icon. Consecutive Perfect placements show a Streak badge. Large scores resize within the score card.

Home, pause and game-over panels share outlined arcade surfaces and raised buttons with a pressed state. Primary actions are gold; Home still offers one Start action and a separate Settings icon. The native Sound/Vibration dialog uses the same palette. Icons are local Canvas vectors; no assets or dependencies were added.

Gameplay, physics, camera, settings persistence and English copy remain unchanged. UI integration selectors now identify Pause by its accessibility label.

During verification, Settings recreation exposed an Espresso root-selection race: the test selected the unfocused activity while the dialog was opening. Settings assertions now explicitly select the dialog window. The collapse lifecycle test now pauses immediately on detecting collapse and captures its transient feedback after resume, avoiding screenshot overhead consuming the 1.5-second collapse interval. All original lifecycle and score assertions remain.

Verification on 2026-10-23:

- `:app:assembleDebug`, `:app:assembleDebugAndroidTest`, `:app:testDebugUnitTest`, and `:app:lintDebug` passed. Two JVM tests passed; lint reported zero errors and nine existing SDK/dependency/orientation warnings.
- Pixel 8 emulator, Android API 37, 1080×2400: all six instrumentation tests passed (55.829 seconds), including navigation, settings recreation, pause during landing/collapse, restart, preferences and atmosphere regression.
- Additional 800×2400 portrait run: menu/play/pause/restart/settings test passed (12.108 seconds). Display size restored afterward.
- Inspected gameplay, home, pause, game over and Settings screenshots. Visual review caught undersized intrinsic-pixel icons and dark dialog option labels; explicit icon scaling and the dialog list text color corrected both. Final screenshots confirm readable labels and unclipped HUD on both sizes.
- Screenshots: [gameplay](screenshots/arcade-ui/game.png), [home](screenshots/arcade-ui/menu.png), [pause](screenshots/arcade-ui/pause.png), [game over](screenshots/arcade-ui/collapse.png), [settings](screenshots/arcade-ui/settings.png), [narrow gameplay](screenshots/arcade-ui/game-narrow.png).

APK: `app/build/outputs/apk/debug/app-debug.apk`. No physical device was available for this UI verification; physical-device performance remains unmeasured.
