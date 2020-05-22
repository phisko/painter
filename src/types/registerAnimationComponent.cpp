#include "helpers/RegisterTypeHelper.hpp"
#include "data/AnimationComponent.hpp"

void registerAnimationComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::AnimationComponent,
		kengine::AnimFilesComponent,
		kengine::AnimListComponent
	>(em);
}
