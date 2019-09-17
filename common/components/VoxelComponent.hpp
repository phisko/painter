#pragma once

#include "Point.hpp"
#include "Pixel.hpp"

struct VoxelComponent {
	putils::Point3i pos;
	Pixel color;

	pmeta_get_class_name(VoxelComponent);
};