#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"

void registerModelComponent() {
	kengine::registerComponents<
		kengine::ModelComponent
	>();
}
