#include "helpers/registerTypeHelper.hpp"
#include "data/SelectedComponent.hpp"

void registerSelectedComponent() {
	kengine::registerComponents<
		kengine::SelectedComponent
	>();
}
