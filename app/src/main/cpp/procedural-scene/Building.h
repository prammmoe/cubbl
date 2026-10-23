#pragma once
#include <cstdint>
#include <vector>
namespace cubbl {
struct Vec3 { float x,y,z; };
struct Color { float r,g,b; };
struct CubePart { Vec3 position, scale; Color color; };
struct BuildingGeometry { std::vector<CubePart> parts; };
BuildingGeometry generateBuilding(int variant,uint32_t seed);
}
