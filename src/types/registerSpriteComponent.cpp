#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

void registerSpriteComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::SpriteComponent2D,
		kengine::SpriteComponent3D
	>(em);
}
