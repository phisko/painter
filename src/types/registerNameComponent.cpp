#include "helpers/registerTypeHelper.hpp"
#include "data/NameComponent.hpp"

void registerNameComponent() {
	kengine::registerComponents<
		kengine::NameComponent
	>();
}
