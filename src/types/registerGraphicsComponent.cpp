#include "helpers/RegisterTypeHelper.hpp"
#include "data/GraphicsComponent.hpp"

void registerGraphicsComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::GraphicsComponent
	>(em);
}
