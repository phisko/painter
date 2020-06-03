#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"

void registerAnimationComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::AnimationComponent,
		kengine::ModelAnimationComponent
	>(em);
}
