#include "helpers/registerTypeHelper.hpp"
#include "data/DebugGraphicsComponent.hpp"

void registerDebugGraphicsComponent() {
	kengine::registerComponents<
		kengine::DebugGraphicsComponent
	>();
}
