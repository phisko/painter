#include "registerTypes.hpp"
#include "data/DebugGraphicsComponent.hpp"

void registerDebugGraphicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::DebugGraphicsComponent
	>(em);
}
