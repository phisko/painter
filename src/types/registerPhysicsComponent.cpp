#include "helpers/registerTypeHelper.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/KinematicComponent.hpp"

void registerPhysicsComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::PhysicsComponent,
		kengine::KinematicComponent
	>(em);
}
