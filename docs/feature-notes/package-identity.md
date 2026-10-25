# Package identity

## 2026-10-25

- Changed application ID and namespace to `com.prammmoelabs.cubbl`.
- Moved main, unit-test, and instrumentation-test Kotlin sources to the matching package directories.
- Renamed JNI entry points and Paparazzi snapshot filenames to match the new package.
- Updated the instrumentation assertion for the installed application ID.
- Verified `:app:assembleDebug`, `:app:compileDebugAndroidTestKotlin`, and `:app:testDebugUnitTest`: BUILD SUCCESSFUL. Device instrumentation was not run.
