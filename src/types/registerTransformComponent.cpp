#include "registerTypes.hpp"
#include "components/TransformComponent.hpp"

void registerTransformComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::TransformComponent3f
	>(em);
}
