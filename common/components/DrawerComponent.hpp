#pragma once

#include "Pixel.hpp"

struct DrawerComponent {
	Pixel color;
	Pixel variation;

	pmeta_get_class_name(DrawerComponent);
	pmeta_get_attributes(
		pmeta_reflectible_attribute(&DrawerComponent::color),
		pmeta_reflectible_attribute(&DrawerComponent::variation)
	);
	pmeta_get_methods();
	pmeta_get_parents();
};