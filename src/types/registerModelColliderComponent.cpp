#include "helpers/registerTypeHelper.hpp"
#include "data/ModelColliderComponent.hpp"

void registerModelColliderComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::ModelColliderComponent
	>(em);
}
