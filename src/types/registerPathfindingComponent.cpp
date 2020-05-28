#include "helpers/registerTypeHelper.hpp"
#include "data/PathfindingComponent.hpp"

void registerPathfindingComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::PathfindingComponent
	>(em);
}
