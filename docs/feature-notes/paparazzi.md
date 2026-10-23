# Paparazzi

## 2026-10-23

Installed the pinned `app.cash.paparazzi` Gradle plugin 2.0.0-alpha05.1 using the existing version catalog, root plugin declaration and app plugin block. This published prerelease adds AGP 9 support; the project keeps AGP 9.3.3, Gradle 9.5 and its existing Java 25 daemon. Paparazzi is a host-side test dependency, not a runtime SDK shipped in the APK.

Added `ArcadeIconSnapshotTest` for the real heart, empty-heart, Pause and Settings drawables. It avoids MainActivity/GameActivity initialization and native library loading. No production UI or gameplay changes. Paparazzi covers Android Views; the GLES tower/world, JNI, audio and full Activity lifecycle continue to need existing native/emulator tests.

Commands:
- Record reviewed golden: `./gradlew :app:recordPaparazziDebug`
- Compare against golden: `./gradlew :app:verifyPaparazziDebug`
- Goldens: `app/src/test/snapshots/`
- Reports: `app/build/reports/paparazzi/debug/`

Sources: https://github.com/cashapp/paparazzi and https://cashapp.github.io/paparazzi/ . No CI, Git LFS or emulator configuration changes were added. This workspace has no Git branch.

Verification:
- `:app:recordPaparazziDebug` passed and produced `app/src/test/snapshots/images/com.example.cubbl_ArcadeIconSnapshotTest_arcadeIcons.png`.
- Inspected the 744×240 golden: the four actual production drawables render correctly against the navy background.
- `:app:verifyPaparazziDebug :app:assembleRelease :app:lintDebug` passed. Three JVM tests passed, including the snapshot test; no emulator was used.
- Signed optimized release APK remains 3,953,506 bytes, the same size as before installing Paparazzi. Rebuilt APK SHA-256: `b71ced5bc664577895fee5a19d41f78458e7d0c5c00c7d1f5881797247d99421`.
- Java 25 reports upstream Byte Buddy Unsafe/layoutlib native-access warnings; they did not fail recording or verification. Existing Kotlin deprecation warnings remain unchanged.

Golden images should be reviewed before re-recording. This smoke test covers the shared icons, not a screenshot of the complete native game.
