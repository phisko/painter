#include "helpers/registerTypeHelper.hpp"
#include "data/ModelSkeletonComponent.hpp"

void registerSkeletonComponent() {
	kengine::registerComponents<
		// kengine::SkeletonComponent,
		kengine::ModelSkeletonComponent
	>();
}
