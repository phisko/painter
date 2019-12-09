#include "registerTypes.hpp"
#include "data/PhysicsComponent.hpp"

void registerPhysicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::PhysicsComponent,
		kengine::KinematicComponent
	>(em);
}
