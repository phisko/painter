#include "registerTypes.hpp"
#include "data/HighlightComponent.hpp"

void registerHighlightComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::HighlightComponent
	>(em);
}
