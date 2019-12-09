#include "registerTypes.hpp"
#include "data/SpriteComponent.hpp"

void registerSpriteComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::SpriteComponent2D,
		kengine::SpriteComponent3D
	>(em);
}
