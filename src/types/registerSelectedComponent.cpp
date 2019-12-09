#include "registerTypes.hpp"
#include "data/SelectedComponent.hpp"

void registerSelectedComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::SelectedComponent
	>(em);
}
