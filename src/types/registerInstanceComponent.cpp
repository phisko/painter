#include "helpers/registerTypeHelper.hpp"
#include "data/InstanceComponent.hpp"

void registerInstanceComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::InstanceComponent
	>(em);
}
