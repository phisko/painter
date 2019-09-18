#include "registerTypes.hpp"
#include "components/AnimationComponent.hpp"

void registerAnimationComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::AnimationComponent,
		kengine::AnimFilesComponent,
		kengine::AnimListComponent
	>(em);
}
