#include "registerTypes.hpp"
#include "data/ModelSkeletonComponent.hpp"

void registerSkeletonComponent(kengine::EntityManager & em) {
	registerComponents<
		// kengine::SkeletonComponent,
		kengine::ModelSkeletonComponent
	>(em);
}
