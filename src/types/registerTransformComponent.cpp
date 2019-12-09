#include "registerTypes.hpp"
#include "data/TransformComponent.hpp"

void registerTransformComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::TransformComponent3f
	>(em);
}
