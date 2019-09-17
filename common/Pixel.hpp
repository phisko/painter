#pragma once

#include "reflection/Reflectible.hpp"
#include "rand.hpp"

struct Pixel {
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 255;

	pmeta_get_class_name(Pixel);
	pmeta_get_attributes(
		pmeta_reflectible_attribute(&Pixel::r),
		pmeta_reflectible_attribute(&Pixel::g),
		pmeta_reflectible_attribute(&Pixel::b),
		pmeta_reflectible_attribute(&Pixel::a)
	);
	pmeta_get_methods();
	pmeta_get_parents();
};

static bool operator==(const Pixel & lhs, const Pixel & rhs) {
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

static bool operator!=(const Pixel & lhs, const Pixel & rhs) {
	return !(lhs == rhs);
}

static void normalize(Pixel & p, int intensity) {
	while (p.r + p.g + p.b != intensity) {
		const auto picked = putils::rand(0, 3);
		auto & toChange = picked == 0 ? p.r : picked == 1 ? p.g : p.b;
		auto diff = intensity - p.r - p.g - p.b;
		diff = std::clamp(diff, -(int)toChange, 255 - toChange);
		toChange += diff;
	}
}

static void varyColor(Pixel & p, unsigned char min, unsigned char max, char variation, int intensity) {
	if (variation == 0)
		return;

	p.r += putils::rand<char>(-variation, variation);
	p.r = std::clamp(p.r, min, max);

	p.g += putils::rand<char>(-variation, variation);
	p.g = std::clamp(p.g, min, max);

	p.b += putils::rand<char>(-variation, variation);
	p.b = std::clamp(p.b, min, max);

	normalize(p, intensity);
}
