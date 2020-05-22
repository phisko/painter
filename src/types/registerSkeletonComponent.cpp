#include "helpers/RegisterTypeHelper.hpp"
#include "data/ModelSkeletonComponent.hpp"

void registerSkeletonComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		// kengine::SkeletonComponent,
		kengine::ModelSkeletonComponent
	>(em);
}
