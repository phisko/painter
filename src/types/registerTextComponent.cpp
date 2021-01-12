#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"

void registerTextComponent() {
	kengine::registerComponents<
		kengine::TextComponent2D,
		kengine::TextComponent3D
	>();
}
