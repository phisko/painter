#include "registerTypes.hpp"
#include "components/ModelSkeletonComponent.hpp"

void registerSkeletonComponent(kengine::EntityManager & em) {
	registerComponents<
		// kengine::SkeletonComponent,
		kengine::ModelSkeletonComponent
	>(em);
}
