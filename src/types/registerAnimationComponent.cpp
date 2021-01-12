#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"
#include "data/ModelAnimationComponent.hpp"

void registerAnimationComponent() {
	kengine::registerComponents<
		kengine::AnimationComponent,
		kengine::ModelAnimationComponent
	>();
}
