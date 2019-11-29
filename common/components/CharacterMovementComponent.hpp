#pragma once

#include "Point.hpp"

struct CharacterMovementComponent {
	putils::Point3f destination;
	float targetDistance = 1.f;

	putils_reflection_class_name(CharacterMovementComponent);
	putils_reflection_attributes(
		putils_reflection_attribute(&CharacterMovementComponent::destination),
		putils_reflection_attribute(&CharacterMovementComponent::targetDistance)
	);
};