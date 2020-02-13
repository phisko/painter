#include "registerTypes.hpp"
#include "data/PyComponent.hpp"

void registerPyComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::PyComponent
	>(em);
}
