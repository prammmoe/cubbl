# Cubbl Privacy Policy — Website Implementation Plan

**Goal:** Buat halaman Privacy Policy Cubbl pada website pribadi yang dapat dipakai sebagai URL privacy policy di Google Play Console.

**Architecture:** Halaman statis mengikuti routing, komponen, dan gaya website yang sudah ada. Tidak perlu backend, database, form, atau dependency baru.

**Tech stack:** Ikuti stack project website tujuan; periksa struktur project sebelum menentukan nama file. Dokumen ini adalah spesifikasi sekaligus rencana implementasi yang dapat dipindahkan tanpa repo Android.

## Instruksi handoff untuk agent website

Implementasikan halaman sesuai dokumen ini di project website yang sedang dibuka. Gunakan pola routing dan design system existing. Kerjakan halaman dan verifikasi lokal terlebih dahulu; laporkan URL final untuk digunakan pemilik di Play Console. Jangan mengubah game Android dari project website. Jangan memublikasikan placeholder sebagai informasi final.

## Identitas dan URL

- Nama app: **Cubbl**.
- Application ID: **com.prammmoelabs.cubbl**.
- Nama brand yang diusulkan: **Prammmoe Labs**. Konfirmasi penulisan identitas developer dengan pemilik dan cocokkan dengan store listing sebelum publikasi; package ID tidak membuktikan nama badan hukum.
- Domain yang tercantum pada Credits game: **https://prammmoe.space/**. Pastikan project website tujuan memang memakai domain ini.
- Route yang direncanakan: **/cubbl/privacy-policy**.
- URL tujuan jika domain dikonfirmasi: **https://prammmoe.space/cubbl/privacy-policy**.
- Judul halaman: **Cubbl Privacy Policy**.
- Bahasa isi: Inggris agar selaras dengan teks game. Gunakan draft di bawah sebagai isi awal.
- Tanggal berlaku: isi dengan tanggal yang disetujui pemilik saat halaman dipublikasikan, bukan tanggal build otomatis.
- Kontak: **[PRIVACY_CONTACT_EMAIL]** harus diganti email aktif milik pemilik. Jangan menebak alamat dari domain.

## Fakta game yang sudah diperiksa

Pemeriksaan source Android pada 25 Oktober 2026 menemukan:

| Area | Perilaku saat ini |
| --- | --- |
| Akun | Tidak ada fitur login atau pembuatan akun |
| Penyimpanan | SharedPreferences lokal bernama `cubbl` |
| Nilai tersimpan | Skor tertinggi, suara aktif/nonaktif, getaran aktif/nonaktif, tutorial selesai/belum |
| Tujuan data | Mengingat progres dan preferensi pemain |
| Izin app | `VIBRATE` untuk efek getaran; manifest source tidak meminta `INTERNET`, lokasi, kamera, mikrofon, atau kontak |
| SDK | Tidak ditemukan integrasi iklan, analytics, atau crash reporting pada source/dependency yang diperiksa |
| Server developer | Tidak ditemukan pengiriman data gameplay ke server developer |
| Backup | `android:allowBackup="true"`; aturan backup tidak mengecualikan SharedPreferences. Android dapat mencadangkan/memulihkan atau mentransfer data sesuai pengaturan perangkat |

Fakta ini perlu dicocokkan kembali dengan build release final jika dependency atau fitur app berubah. Jangan mengklaim data selalu berada di satu perangkat karena backup Android aktif. Jangan mengklaim data dienkripsi oleh app; penyimpanan memakai sandbox Android dan SharedPreferences biasa.

## Batasan halaman

- Halaman HTML publik melalui HTTPS, dapat dibaca tanpa login, pembayaran, atau download PDF.
- Teks dapat diakses langsung; jangan letakkan seluruh kebijakan di modal atau di balik tombol persetujuan.
- Gunakan layout artikel sederhana, heading semantik, lebar baca nyaman, kontras dan fokus keyboard yang jelas, serta tampilan mobile yang rapi.
- Tambahkan tautan bernama **Cubbl Privacy Policy** pada footer atau area legal yang sesuai tanpa merombak navigasi website.
- Gunakan title metadata dan canonical URL mengikuti helper SEO project; tidak memerlukan social image baru.
- Jangan menambah analytics, tracker, cookie, atau form baru untuk halaman ini.
- Audit analytics/cookie/hosting website existing. Kebijakan app dan pengolahan data pengunjung website merupakan konteks berbeda; bila website mengolah data, tautkan kebijakan website yang benar atau tambahkan penjelasan yang sudah diverifikasi. Jangan mengarang klaim bahwa website tidak mencatat IP atau tidak memakai cookie.
- Tidak perlu fitur penghapusan akun karena Cubbl saat ini tidak memiliki akun.

## Draft konten halaman

Teks berikut adalah isi pengguna, bukan instruksi implementasi. Ganti semua placeholder sebelum publikasi.

---

# Cubbl Privacy Policy

Effective date: [EFFECTIVE_DATE]

This Privacy Policy explains how [CONFIRMED_DEVELOPER_NAME] handles information in Cubbl, the Android game with package name com.prammmoelabs.cubbl.

## Information used by the game

Cubbl does not require an account or ask you to provide your name, email address, location, contacts, photos, or payment information. The current version does not send gameplay information to servers operated by the developer and does not include advertising, analytics, or third-party crash-reporting services.

## Information stored on your device

Cubbl saves your highest score, sound preference, vibration preference, and tutorial completion status in the app's local storage. This information is used to remember your progress and settings between play sessions.

## Android backup and device transfer

Android may back up, restore, or transfer the app's saved information depending on your device, backup provider, and system settings. These processes are managed by Android and your backup provider, rather than by a Cubbl server. You can manage available backup options in your device settings.

## Permissions

Cubbl uses the device's vibration feature for gameplay feedback. You can turn vibration off in the game's settings. Cubbl does not request access to your location, camera, microphone, or contacts.

## Sharing and sale of information

The developer does not receive, sell, or share your locally saved gameplay information. System-managed backups and device transfers are described above.

## Retention and deletion

Your score and preferences remain in the app's storage until that storage is cleared or the app is uninstalled. To remove the local data, open Android Settings, select Apps, then Cubbl, then Storage, and choose Clear storage or Clear data. The exact labels may differ by device. This resets your saved score and preferences.

Removing local data does not necessarily remove a system backup. Backup copies and restoration are controlled by your device and backup provider; saved data may return if you restore a backup. The developer does not operate a server copy of your gameplay data.

## Children’s privacy

Cubbl does not ask players to submit personal information and does not include account registration, advertising, or analytics in its current version. If you believe a child has provided personal information to the developer through a support message, please contact us using the address below.

## External websites and support

If you open an external website, that website's privacy practices apply. If you email us for support or a privacy question, we receive the email address and information you choose to include in your message.

## Changes to this policy

We may update this policy when the app's features or information practices change. The effective date above identifies the current policy.

## Contact

For privacy questions about Cubbl, contact [CONFIRMED_DEVELOPER_NAME] at [PRIVACY_CONTACT_EMAIL].

---

## Task 1: Sesuaikan identitas dan konteks website

- [ ] Periksa routing, layout artikel/legal, metadata, footer, dan scripts global pada repo website tujuan.
- [ ] Tentukan file route dan file footer yang tepat berdasarkan struktur nyata; jangan membuat framework atau layout paralel.
- [ ] Konfirmasi domain, nama developer pada store listing, email kontak, dan tanggal berlaku dengan pemilik. Implementasi lokal boleh berjalan dengan placeholder yang jelas; publikasi harus menunggu nilai final.
- [ ] Jika kontak dukungan benar-benar digunakan, konfirmasi praktik penyimpanan email dukungan dan jelaskan tujuan, penerima/penyedia email, retensi, serta cara meminta penghapusan secara akurat. Jangan membuat janji waktu retensi tanpa praktik yang mendukungnya.
- [ ] Cocokkan paragraf anak-anak dengan target audiens yang dipilih di Play Console; jangan menambahkan batas usia 13 tahun secara otomatis.

## Task 2: Implementasikan halaman dan aksesnya

- [ ] Buat route `/cubbl/privacy-policy` memakai layout existing dan isi draft yang telah disesuaikan.
- [ ] Jadikan email kontak tautan `mailto:` dan tampilkan app name, developer, serta tanggal dengan jelas.
- [ ] Tambahkan title metadata dan canonical URL memakai domain yang sudah dikonfirmasi.
- [ ] Tambahkan link footer/legal ke route tersebut.
- [ ] Sesuaikan penjelasan data pengunjung website berdasarkan scripts dan layanan nyata yang ditemukan pada Task 1.

## Task 3: Verifikasi dan handoff publikasi

- [ ] Jalankan build/lint yang memang disediakan repo website dan catat hasilnya. Tidak perlu membuat framework test baru untuk halaman statis ini.
- [ ] Buka URL langsung pada viewport mobile dan desktop; pastikan teks terbaca, tidak terpotong, heading terurut, serta link dapat digunakan lewat keyboard.
- [ ] Periksa HTML hasil render agar berisi kebijakan lengkap; pastikan tidak ada placeholder, klaim yang bertentangan dengan backup, atau email palsu.
- [ ] Pastikan link footer, canonical URL, dan `mailto:` benar.
- [ ] Setelah deployment yang diotorisasi pemilik, uji URL HTTPS publik lewat sesi tanpa login: respons sukses, isi lengkap, tanpa pembatasan geografis atau login.
- [ ] Kembalikan URL final, ringkasan file yang berubah, hasil verifikasi, dan hal yang masih memerlukan keputusan pemilik.

## Tindak lanjut pada project Android / Play Console

Bagian ini merupakan handoff untuk pemilik dan project Android, bukan pekerjaan yang harus dijalankan agent website:

- Masukkan URL publik final pada kolom Privacy Policy di Play Console.
- Tambahkan tautan **Privacy Policy** pada Settings/Credits Cubbl; halaman website saja belum menyediakan akses dari dalam app.
- Cocokkan formulir Data safety dengan build release dan SDK final; halaman ini tidak otomatis mengisi atau memvalidasi formulir tersebut.
- Perbarui kebijakan sebelum merilis fitur baru yang mengubah pengolahan data, misalnya iklan, analytics, login, atau leaderboard online.

## Referensi untuk pengecekan ulang

- [Google Play — User Data / Privacy Policy](https://support.google.com/googleplay/android-developer/answer/10144311?hl=en): ketentuan tautan publik, identitas/kontak, praktik data, retensi/penghapusan, serta akses kebijakan dari app.
- [Android — Auto Backup](https://developer.android.com/identity/data/autobackup): SharedPreferences dapat termasuk dalam backup sistem dan dipulihkan saat instalasi.

Dokumen ini menyiapkan implementasi berdasarkan perilaku kode saat diperiksa; bukan klaim bahwa halaman atau app telah disetujui Google Play.
