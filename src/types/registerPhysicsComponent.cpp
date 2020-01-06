#include "registerTypes.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/KinematicComponent.hpp"

void registerPhysicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::PhysicsComponent,
		kengine::KinematicComponent
	>(em);
}
