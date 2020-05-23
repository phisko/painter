#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"

void registerTextComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::TextComponent2D,
		kengine::TextComponent3D
	>(em);
}
