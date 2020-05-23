#include "helpers/registerTypeHelper.hpp"
#include "data/NavMeshComponent.hpp"

void registerNavMeshComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::NavMeshComponent,
		kengine::RebuildNavMeshComponent
	>(em);
}
