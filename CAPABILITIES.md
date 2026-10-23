# Cubbl capability map
| ID | Responsibility | Dependencies |
|---|---|---|
| game-core | Session rules and concrete PhysicsWorld | standard C++, vendored Box2D 3.1.1 |
| procedural-scene | Cube parts and pastel variants | game-core |
| native-runtime | Rendering, loop, input, lifecycle | game-core, procedural-scene |
| player-experience | Android UI, audio, persistence | native-runtime, game-core |
Build order: domain, scene, runtime, experience; integrate a playable cube before art polish.
