#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"
#include "data/TextureModelComponent.hpp"

void registerModelComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::ModelComponent,
		kengine::TextureModelComponent
	>(em);
}
