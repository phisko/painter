#include "registerTypes.hpp"
#include "data/ModelComponent.hpp"
#include "data/TextureModelComponent.hpp"

void registerModelComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::ModelComponent,
		kengine::TextureModelComponent
	>(em);
}
