#include "helpers/registerTypeHelper.hpp"
#include "data/DebugGraphicsComponent.hpp"

void registerDebugGraphicsComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::DebugGraphicsComponent
	>(em);
}
