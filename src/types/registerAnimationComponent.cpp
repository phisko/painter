#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"
#include "data/AnimationFilesComponent.hpp"
#include "data/ModelAnimationComponent.hpp"

void registerAnimationComponent() {
	kengine::registerComponents<
		kengine::AnimationComponent,
		kengine::AnimationFilesComponent,
		kengine::ModelAnimationComponent
	>();
}
