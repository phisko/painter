#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"

void registerAdjustableComponent() {
	kengine::registerComponents<
		kengine::AdjustableComponent
	>();
}
