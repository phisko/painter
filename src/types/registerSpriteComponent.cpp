#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

void registerSpriteComponent() {
	kengine::registerComponents<
		kengine::SpriteComponent2D,
		kengine::SpriteComponent3D
	>();
}
