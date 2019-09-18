#include "registerTypes.hpp"
#include "components/GodRaysComponent.hpp"

void registerGodRaysComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::GodRaysComponent
	>(em);
}
