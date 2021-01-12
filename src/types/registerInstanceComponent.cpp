#include "helpers/registerTypeHelper.hpp"
#include "data/InstanceComponent.hpp"

void registerInstanceComponent() {
	kengine::registerComponents<
		kengine::InstanceComponent
	>();
}
