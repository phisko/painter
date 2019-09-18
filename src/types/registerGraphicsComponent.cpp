#include "registerTypes.hpp"
#include "components/GraphicsComponent.hpp"

void registerGraphicsComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::GraphicsComponent
	>(em);
}
