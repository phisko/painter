#include "helpers/registerTypeHelper.hpp"
#include "data/GodRaysComponent.hpp"

void registerGodRaysComponent() {
	kengine::registerComponents<
		kengine::GodRaysComponent
	>();
}
