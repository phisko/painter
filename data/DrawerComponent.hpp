#pragma once

#include "Pixel.hpp"

struct DrawerComponent {
	Pixel color;
	Pixel variation;

	putils_reflection_class_name(DrawerComponent);
	putils_reflection_attributes(
		putils_reflection_attribute(&DrawerComponent::color),
		putils_reflection_attribute(&DrawerComponent::variation)
	);
	putils_reflection_methods();
	putils_reflection_parents();
};