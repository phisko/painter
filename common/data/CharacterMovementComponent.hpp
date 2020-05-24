#pragma once

#include "Point.hpp"
#include "Entity.hpp"
#include "data/NavMeshComponent.hpp"

struct CharacterMovementComponent {
	putils::Point3f destination;
	float targetDistance = 1.f;

	putils_reflection_class_name(CharacterMovementComponent);
	putils_reflection_attributes(
		putils_reflection_attribute(&CharacterMovementComponent::destination),
		putils_reflection_attribute(&CharacterMovementComponent::targetDistance)
	);
};

struct PathComponent {
	kengine::Entity::ID navMesh = kengine::Entity::INVALID_ID;
	kengine::NavMeshComponent::Path path;
	size_t currentStep = 0;

	putils_reflection_class_name(PathComponent);
	putils_reflection_attributes(
		putils_reflection_attribute(&PathComponent::navMesh),
		putils_reflection_attribute(&PathComponent::path),
		putils_reflection_attribute(&PathComponent::currentStep)
	);
};