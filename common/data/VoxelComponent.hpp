#pragma once

#include "Point.hpp"
#include "Pixel.hpp"

struct VoxelComponent {
	putils::Point3i pos;
	Pixel color;

	putils_reflection_class_name(VoxelComponent);
};