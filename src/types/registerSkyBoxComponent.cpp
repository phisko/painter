#include "registerTypes.hpp"
#include "data/SkyBoxComponent.hpp"

void registerSkyBoxComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::SkyBoxComponent
	>(em);
}
