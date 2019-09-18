#include "registerTypes.hpp"
#include "components/SkeletonComponent.hpp"

void registerSkeletonComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::SkeletonComponent,
		kengine::ModelSkeletonComponent
	>(em);
}
