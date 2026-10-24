# Asset license audit — Cubbl

Reviewed: 2026-10-24. This is a repository evidence review, not legal clearance or a guarantee of Google Play approval.

| Shipped asset/component | Evidence | Assessment |
| --- | --- | --- |
| Procedural houses, city, environment, UI icons | Local C++ procedural-scene/Renderer and Kotlin ArcadeIcon source; no third-party scene textures found | Locally implemented. Ownership of contributions and any design references is assumed, not independently verified. |
| Launcher artwork and city launch background | Generated with imagegen in this chat; source PNGs retained under assets/icon | OpenAI terms assign output rights to the user as between user and OpenAI, subject to law/terms. Output may not be unique and third-party rights remain relevant. The supplied game screenshot is assumed to be owned or licensed by the user. No trademark search or image similarity search was performed. |
| Baloo 2 font | app/src/main/assets/licenses/Baloo2-OFL.txt; upstream EkType license | SIL OFL 1.1 permits embedding/commercial distribution subject to its conditions. License and copyright notice bundled and accessible through Credits. |
| miss.wav / collapse.wav | docs/audio/CREDITS.md and failure-sounds-processing.json; Kenney Impact Sounds and Breaking Rock sources | Source pages identify CC0. Source hashes and processing recipes are recorded. Audio provenance and Kenney license now also bundled. |
| streak.wav | docs/audio/generate-streak.py and audio credits | Documented original synthesized chime; no external recording. |
| drop.wav / land.wav / perfect.wav | Present since initial commit; no source or generation recipe found in repository history/docs | NOT VERIFIED. Record their actual origin and applicable permission, or replace with newly generated/original sounds before treating the asset audit as complete. A simple waveform alone does not prove ownership. |
| Box2D 3.1.1 | Vendored MIT license, version/source/archive hash in vendor README | Commercial use permitted with the license conditions. MIT copyright/permission notice copied into shipped assets and shown in Credits. Upstream sample assets, including Droid Sans, are not part of the configured native build. |
| AndroidX, Material, GameActivity and transitive dependencies | Gradle declarations | Full SDK/transitive license/NOTICE audit has NOT been completed. Do not interpret this asset review as clearance of every dependency. |

## Changes from this review

- Added bundled `Box2D-MIT.txt`, `Kenney-Impact-Sounds-License.txt` and `Audio-Credits.txt` beside the existing font license.
- Added an in-app Asset licenses viewer from Credits.
- No audio was replaced and no unclear provenance was relabeled as original.

## Before publishing

1. Resolve the three undocumented sound effects above.
2. Review licenses/NOTICE requirements for all packaged production dependencies, including native/static dependencies.
3. Check the Cubbl name and store listing artwork for trademark/third-party rights. This review does not establish trademark availability or copyright exclusivity.
4. Complete Play Console policy, privacy/Data safety, age/content declarations and release signing separately; these are outside this asset audit.

## Primary sources

- OpenAI Terms of Use: https://openai.com/policies/terms-of-use/
- Google Play intellectual property policy: https://support.google.com/googleplay/android-developer/answer/9888072?hl=en
- Baloo 2 OFL: https://github.com/EkType/Baloo2/blob/master/OFL.txt
- Kenney Impact Sounds CC0: https://kenney.nl/assets/impact-sounds
- Breaking Rock CC0: https://opengameart.org/content/breaking-rock
- Box2D license: app/src/main/cpp/vendor/box2d/LICENSE
