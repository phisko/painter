#include "helpers/registerTypeHelper.hpp"
#include "data/ShaderComponent.hpp"

void registerShaderComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::GBufferShaderComponent,
		kengine::LightingShaderComponent,
		kengine::PostLightingShaderComponent,
		kengine::PostProcessShaderComponent,
		kengine::ShadowCubeShaderComponent,
		kengine::ShadowMapShaderComponent,
		kengine::DepthCubeComponent,
		kengine::DepthMapComponent
	>(em);
}
