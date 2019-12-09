#include "registerTypes.hpp"
#include "data/NameComponent.hpp"

void registerNameComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::NameComponent
	>(em);
}
