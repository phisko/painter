#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"

void registerSkyBoxComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::SkyBoxComponent
	>(em);
}
