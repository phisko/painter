#include "registerTypes.hpp"
#include "components/SelectedComponent.hpp"

void registerSelectedComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::SelectedComponent
	>(em);
}
