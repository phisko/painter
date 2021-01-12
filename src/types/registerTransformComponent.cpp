#include "helpers/registerTypeHelper.hpp"
#include "data/TransformComponent.hpp"

void registerTransformComponent() {
	kengine::registerComponents<
		kengine::TransformComponent
	>();
}
