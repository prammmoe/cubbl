# Failure sound credits

Bundled in `app/src/main/res/raw/` for offline playback. All source recordings below are CC0 1.0; attribution is retained for provenance.

- **Kenney — Impact Sounds 1.0**: https://kenney.nl/assets/impact-sounds . Used `impactWood_heavy_000.ogg`, `impactSoft_heavy_000.ogg`, and `impactMining_000.ogg`. Original license is retained in `Kenney-Impact-Sounds-License.txt`.
- **themightyglider — Breaking Rock**: https://opengameart.org/content/breaking-rock . Source file `rock_break_0.ogg`, derivative of **SoundCollectah — stone falls and breaks low pitch.aiff**: https://freesound.org/people/SoundCollectah/sounds/109360/ . Both pages identify the audio as CC0.
- License: https://creativecommons.org/publicdomain/zero/1.0/ . Downloaded 2026-10-23.

Adaptations for Cubbl:
- `miss.wav` (0.50 s): short heavy wood/soft impact blend, filtered and faded.
- `collapse.wav` (1.40 s): mining impact plus staggered, lowered-pitch rock cracks and a wood impact; fading debris tail.
- Both are 44.1 kHz mono PCM16, peak normalized to 0.85. Source SHA-256 hashes and exact FFmpeg filter recipes are in `failure-sounds-processing.json`; output uses `-ar 44100 -ac 1 -c:a pcm_s16le` and peak normalization afterward.

No audio services, runtime downloads or production dependencies are required.

## Streak activation chime
`streak.wav` is an original synthesized four-note rising chime (C5–E5–G5–C6), generated locally by `python3 docs/audio/generate-streak.py` using only the Python standard library. No external recording is used. It is 0.48 seconds, mono PCM16 at 22.05 kHz, peak 0.70, with short attack/release envelopes. It replaces the normal Perfect sound when the visible streak activates, and follows the existing Sound/mute/focus/pause controls.
