#include "registerTypes.hpp"
#include "data/GodRaysComponent.hpp"

void registerGodRaysComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::GodRaysComponent
	>(em);
}
