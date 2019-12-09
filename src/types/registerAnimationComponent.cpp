#include "registerTypes.hpp"
#include "data/AnimationComponent.hpp"

void registerAnimationComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::AnimationComponent,
		kengine::AnimFilesComponent,
		kengine::AnimListComponent
	>(em);
}
