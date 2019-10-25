#include "registerTypes.hpp"
#include "components/ModelComponent.hpp"
//#include "components/TextureModelComponent.hpp"

void registerModelComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::ModelComponent//,
		// kengine::TextureModelComponent
	>(em);
}
