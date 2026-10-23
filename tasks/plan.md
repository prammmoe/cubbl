# Rumah miring dan tower rubuh — rencana disetujui 2026-10-23

Urutan: dependency lokal → adapter PhysicsWorld → pose/renderer → pendaratan/skor → collapse → kamera/culling → JNI/UI/lifecycle → regresi.
Box2D 3.1.1 adalah satu dependency produksi baru yang disetujui. Source dan lisensi disimpan lokal; konfigurasi build tidak mengunduh apa pun. Empat modul dipertahankan. State publik tidak mengekspos Box2D.

Collider 2.4 × 1.3, foundation 3.1, massa seragam, friction .8, restitution 0, gravitasi -12. Fixed step 1/60 dengan empat substep. Semua rumah terpasang dinamis. Release membuat body tegak tanpa kecepatan awal. Tidak ada snap atau bounce buatan.
Target spawn adalah rumah diterima dengan titik collider tertinggi (foundation untuk pertama). Gantungan berpusat pada target X dengan jarak dasar 2.6 di atas top target. Dukungan dari bawah normalY >= .5 harus tersambung ke foundation dan mengenai target selama .35 detik terus menerus. Grade memakai offset tangensial dan selisih sudut terpendek: Perfect <=5%/3°, Good <=15%/8°, lainnya Bad. Skor/combo/speed existing dipertahankan.
Miss: tanpa dukungan target dan collider seluruhnya turun satu tinggi rumah dari top target saat spawn. Penalti sekali, recovery .8 detik dengan fisika tetap berjalan; body gagal dihapus sebelum spawn.
Collapse diperiksa sebelum skor/miss: rumah diterima bersudut absolut >=45° selama .2 detik, atau tanpa jalur foundation dan turun >=.65 dari Y terakhir bertumpu selama .2 detik. Timer pulih saat syarat hilang. Collapsing ditambahkan sebagai fase 7; simulasikan 1.5 detik, lalu GameOver/Collapse tanpa menghabiskan nyawa. Pause membekukan dunia dan timer. Identitas stabil dan HousePose berpusat pada body; alasan akhir ditambahkan sebagai snapshot JNI indeks 8.
Renderer memutar posisi part dan normal terhadap pusat body; kamera mengikuti X/Y nyata dan berhenti mengikuti saat collapse. Culling berdasarkan bounds rotasi. UI menampilkan Tower rubuh! dan alasan akhir, efek miss sekali, penyimpanan tidak berubah.

Verifikasi: CTest host, APK semua ABI existing, JVM/lint, instrumentasi emulator bila tersedia. Wajib 50 rumah terpusat stabil, kontak/rotasi, debounce collapse, prioritas penalti, pause/restart, determinisme 30/60/120 FPS. Pengukuran 60 FPS perangkat fisik dilaporkan terpisah.

Measured tuning: contact damping ratio 2 and sleep threshold 1; contact solver remains 30 Hz, four substeps. No fixed rotation, pose correction, joint stabilization or static replacement is used.

2026-10-23 difficulty extension: SPEC-game-core.md now specifies placement-dependent physical sway and per-floor crane speed up to 3×. Details and verification: docs/feature-notes/height-difficulty.md.

## Fixed camera and grounded construction site (2026-10-23)
1. Fix zoom to surface aspect and test moving/rotating/tall stacks and pause/resize.
2. Add separate ground collider and contact query; keep foundation support graph isolated.
3. Route terrain contact through one miss recovery; preserve collapse priority and score semantics.
4. Replace the floating base with cached construction terrain, low concrete foundation, fences and props.
5. Run host/Android checks and inspect emulator screenshots; record physical-device limitations.
