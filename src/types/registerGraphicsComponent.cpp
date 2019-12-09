#include "registerTypes.hpp"
#include "data/GraphicsComponent.hpp"

void registerGraphicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::GraphicsComponent
	>(em);
}
