# Rumah miring dan tower rubuh
- [x] 1. Vendor Box2D 3.1.1, lisensi, static PIC; host/Android build.
- [x] 2. PhysicsWorld: body, pose, kontak, step/reset; tes host.
- [x] 3. Migrasi HousePose dan renderer; tes pivot/rotasi.
- [x] Checkpoint A: satu rumah jatuh/bertumpu; host dan APK.
- [x] 4. Dukungan .35s, grading lokal, skor sekali, miss/recovery.
- [x] 5. Debounce collapse, prioritas, Collapsing 1.5s; tes.
- [x] 6. Kamera X/Y, framing dan culling bounds rotasi.
- [x] Checkpoint B: game over melalui nyawa dan collapse.
- [x] 7. JNI 9 elemen, UI alasan, audio, lifecycle/instrumentasi.
- [x] 8. Regresi 50 rumah, determinisme, restart, dokumentasi hasil.

- [ ] Pengukuran 60 FPS dan profiling GPU pada ponsel fisik — perangkat fisik belum tersedia.

## Fixed camera and grounded site (2026-10-23)
- [x] Aspect-only zoom and camera regression coverage.
- [x] Terrain collider, isolated support graph and one-shot ground miss recovery.
- [x] Grounded concrete foundation, fencing, gate and construction props.
- [x] Host, sanitizer, Android build, JVM, lint and full instrumentation checks.
- [x] Inspect final standard/narrow portrait screenshots (1080×2400 and 800×2400); restore emulator resolution.
- [ ] Measure 60 FPS on a physical phone (not available).
