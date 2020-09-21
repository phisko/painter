#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"

void registerModelComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::ModelComponent
	>(em);
}
