#include "helpers/registerTypeHelper.hpp"
#include "data/PathfindingComponent.hpp"

void registerPathfindingComponent() {
	kengine::registerComponents<
		kengine::PathfindingComponent
	>();
}
