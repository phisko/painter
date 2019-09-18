#include "registerTypes.hpp"
#include "components/DebugGraphicsComponent.hpp"

void registerDebugGraphicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::DebugGraphicsComponent
	>(em);
}
