#include "registerTypes.hpp"
#include "components/SkyBoxComponent.hpp"

void registerSkyBoxComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::SkyBoxComponent
	>(em);
}
