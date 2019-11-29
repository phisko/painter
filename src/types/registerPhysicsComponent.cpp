#include "registerTypes.hpp"
#include "components/PhysicsComponent.hpp"

void registerPhysicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::PhysicsComponent,
		kengine::KinematicComponent
	>(em);
}
