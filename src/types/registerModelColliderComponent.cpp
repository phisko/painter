#include "helpers/registerTypeHelper.hpp"
#include "data/ModelColliderComponent.hpp"

void registerModelColliderComponent() {
	kengine::registerComponents<
		kengine::ModelColliderComponent
	>();
}
