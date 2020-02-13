#include "registerTypes.hpp"
#include "data/ModelColliderComponent.hpp"

void registerModelColliderComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::ModelColliderComponent
	>(em);
}
