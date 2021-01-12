#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"

void registerSkyBoxComponent() {
	kengine::registerComponents<
		kengine::SkyBoxComponent
	>();
}
