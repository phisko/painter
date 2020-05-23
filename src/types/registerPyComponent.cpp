#include "helpers/registerTypeHelper.hpp"
#include "data/PyComponent.hpp"

void registerPyComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::PyComponent
	>(em);
}
