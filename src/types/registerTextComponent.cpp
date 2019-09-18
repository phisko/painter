#include "registerTypes.hpp"
#include "components/TextComponent.hpp"

void registerTextComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::TextComponent2D,
		kengine::TextComponent3D
	>(em);

	registerTypes<
		kengine::TextComponent::string
	>(em);
}
