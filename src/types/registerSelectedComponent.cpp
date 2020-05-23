#include "helpers/registerTypeHelper.hpp"
#include "data/SelectedComponent.hpp"

void registerSelectedComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::SelectedComponent
	>(em);
}
