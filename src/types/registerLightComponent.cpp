#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"

void registerLightComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::DirLightComponent,
		kengine::PointLightComponent,
		kengine::SpotLightComponent
	>(em);
}
