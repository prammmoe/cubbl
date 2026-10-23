# Spec: player-experience

## Objective
Accessible English UI with short arcade copy and local feedback. Audience: casual players using one hand on portrait Android 11+ phones. Offline only.

## Stack and structure
MainActivity.kt, PlayerPreferences.kt, GameAudio.kt; Android/JVM tests. Existing GameActivity/GLES3/CMake stack; no new production dependencies.

## Commands
- Android build: `./gradlew :app:assembleDebug`
- JVM and lint: `./gradlew :app:testDebugUnitTest :app:lintDebug`
- Native: `cmake -S app/src/test/cpp -B /tmp/cubbl-core-tests -DCMAKE_BUILD_TYPE=Debug && cmake --build /tmp/cubbl-core-tests && ctest --test-dir /tmp/cubbl-core-tests --output-on-failure`
- Android integration: `./gradlew :app:connectedDebugAndroidTest`

## Code style
Use installed Android Views, SoundPool and SharedPreferences. UI-thread-only views; no domain rules in UI.
Example boundary: `PlacementGrade classifyPlacement(float offset, float width);`
Use descriptive names, small functions, RAII and explicit state transitions.

## Testing and success criteria
Menu/play/pause/restart/game-over flows; safe insets and 48dp controls; persistent nondecreasing best, sound/vibration toggles, first-placement tutorial.
Host tests prove domain/time/geometry. Android tests prove navigation and persistence. Runtime visual checks and physical-device performance measurements are recorded separately.

## Boundaries
- Always: keep dependency direction inward, validate JNI and persisted values, run relevant checks.
- Ask before: changing engine, adding production dependencies, monetization or online services.
- Never: claim unmeasured performance, hide failing checks, add speculative frameworks, store secrets.

## Open questions
None blocking implementation. Physical-device model is selected from devices actually available; emulator results do not establish physical 60 FPS.

Snapshot has nine elements. Collapsing is phase 7; show “Tower rubuh!” and play miss effect once. GameOver shows Collapse/OutOfLives reason. No preference migration.

## English arcade copy (2026-10-23)
All player-facing text, dynamic score/lives/settings and accessibility instructions use Android string resources. Tone is playful and direct: Start stacking, Keep stacking, Perfect!, Nice drop!, Close call!, Miss!, Timber!, Run over!, Stack again. Perfect combos are labeled Streak rather than implying a score multiplier. Game-over reasons remain explicit: Tower down! or Out of lives!

## Minimal home and settings (2026-10-23)
Home has one primary Start action. A separate 48dp configuration icon in the safe top-right corner opens a standard Settings dialog with Sound and Vibration checkboxes. Changes persist immediately; muting stops active audio. Done/Back close settings, and open settings plus preferences survive Activity recreation. Labels remain English and the icon has a Settings accessibility description.

## Arcade UI (2026-10-23)
Use bundled offline Baloo 2 for every text element, including native Settings. Cards use Sage & Ivory: ivory #F5F0DF panels, sage #BBC9AE fields, dark #344B46 text/lower edges and vibrant cherry-red #C72E48 hearts. Primary buttons use Muted Terracotta #C58B76 with #453D3A text/lower edges. Rounded faces have no contrasting outline at rest; pressed faces move down and hide the backing edge entirely; releasing/cancelling restores the lower edge. Keyboard focus adds a dark outline. Home keeps only Start plus a separate Settings icon. Gameplay uses one compact top row: three heart icons, a large numeric score with Best underneath, and a 52dp icon-only Pause control. Lost hearts become outlines; a Streak text line appears after consecutive Perfect placements. Preserve score/lives accessibility labels, English copy, safe insets, 48dp minimum controls and large-score auto-sizing. Style the existing native Settings dialog consistently. No gameplay, physics, storage or dependency changes.

## Failure audio (2026-10-23)
Miss and Collapsed events use distinct offline WAV samples: a 0.5-second impact and a 1.4-second crack/debris sequence respectively. CC0 sources and processing are documented in docs/audio/CREDITS.md. Collapse plays once on its existing event sequence, retains failure vibration, and follows Sound/focus/pause behavior. No runtime network or new production dependency.

## Paparazzi scene coverage (2026-10-23)
MainActivity and host screenshot tests share GameOverlay and GameOverlayState. The Activity owns JNI, preferences, input debouncing, timers and audio/haptics; GameOverlay owns Android View construction and display updates. Paparazzi captures home, tutorial, gameplay, pause, collapse and both game-over reasons using the actual overlay over static GLES frames copied directly from the native SurfaceView with PixelCopy by instrumentation. These test-only backgrounds do not execute native physics or replace emulator lifecycle/rendering checks. Re-record backgrounds explicitly when native artwork changes; review both fixtures and golden images before accepting updates.

## Baloo 2 and pastel UI (2026-10-23)
Baloo 2 variable weights 400–800 are bundled from Google Fonts with the SIL OFL license in assets/licenses/Baloo2-OFL.txt. Use medium body text, bold buttons and extra-bold score/headings. Day and night configurations share the same palette/font; weather only changes the native scene. Shared color resources drive production UI and icon snapshots. Preserve controls, state, sizing, safe insets and English copy.

## Larger gameplay HUD (2026-10-23)
HUD labels use 13sp, score auto-sizes up to 42sp, Best up to 15sp, and Streak uses the same 28sp feedback style as Perfect. Score rows reserve 18/56/22dp inside a 112dp card to avoid clipping the Baloo 2 metrics. Hearts are 22×24dp in a 92dp lives card. Keep long-score auto-sizing and separate feedback from the taller HUD. Cherry-red hearts use a dedicated ui_heart color so the rest of the pastel palette stays unchanged.

## Streak feedback and sound (2026-10-23)
Show persistent Streak N (N ≥ 2) directly above placement feedback in one centered vertical group below the HUD. Both lines use exactly the same 28sp extra-bold Baloo 2, ivory color and dark shadow; Streak has no card, forced uppercase or extra letter spacing. Clear/reset visibility follows the existing combo state. Play the local 480ms rising chime only when a new Perfect event changes combo from below two to at least two. Later Perfect events keep the normal Perfect sound. Use existing sequence deduplication, sound preference, focus and pause behavior; recreation never replays a historical event. No JNI/gameplay rule changes.

## Settings button surface (2026-10-23)
The home Settings icon button uses the same ivory ui_surface fill as the menu panel, with its existing dark symbol, lower edge, focus and pressed states. The Pause button retains its sage fill.
