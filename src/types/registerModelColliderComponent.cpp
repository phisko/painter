#include "registerTypes.hpp"
#include "components/ModelColliderComponent.hpp"

void registerModelColliderComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::ModelColliderComponent
	>(em);

	registerTypes<
		kengine::ModelColliderComponent::Collider
	>(em);
}
