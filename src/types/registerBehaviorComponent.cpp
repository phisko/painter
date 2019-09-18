#include "registerTypes.hpp"
#include "components/BehaviorComponent.hpp"

void registerBehaviorComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::BehaviorComponent
	>(em);
}
