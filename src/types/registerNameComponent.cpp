#include "helpers/RegisterTypeHelper.hpp"
#include "data/NameComponent.hpp"

void registerNameComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::NameComponent
	>(em);
}
