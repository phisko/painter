#include "registerTypes.hpp"
#include "data/LightComponent.hpp"

void registerLightComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::DirLightComponent,
		kengine::PointLightComponent,
		kengine::SpotLightComponent
	>(em);
}
