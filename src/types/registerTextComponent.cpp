#include "registerTypes.hpp"
#include "data/TextComponent.hpp"

void registerTextComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::TextComponent2D,
		kengine::TextComponent3D
	>(em);
}
