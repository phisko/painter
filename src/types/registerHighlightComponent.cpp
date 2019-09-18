#include "registerTypes.hpp"
#include "components/HighlightComponent.hpp"

void registerHighlightComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::HighlightComponent
	>(em);
}
