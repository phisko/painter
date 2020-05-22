#include "helpers/RegisterTypeHelper.hpp"
#include "data/TransformComponent.hpp"

void registerTransformComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::TransformComponent
	>(em);
}
