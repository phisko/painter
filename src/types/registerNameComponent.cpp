#include "registerTypes.hpp"
#include "components/NameComponent.hpp"

void registerNameComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::NameComponent
	>(em);
}
