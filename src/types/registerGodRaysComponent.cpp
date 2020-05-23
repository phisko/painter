#include "helpers/registerTypeHelper.hpp"
#include "data/GodRaysComponent.hpp"

void registerGodRaysComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::GodRaysComponent
	>(em);
}
