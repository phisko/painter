#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"

void registerAdjustableComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::AdjustableComponent
	>(em);
}
