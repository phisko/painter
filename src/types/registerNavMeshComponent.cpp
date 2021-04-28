#include "helpers/registerTypeHelper.hpp"
#include "data/NavMeshComponent.hpp"

void registerNavMeshComponent() {
	kengine::registerComponents<
		kengine::NavMeshComponent
	>();
}
