#include "helpers/RegisterTypeHelper.hpp"
#include "data/HighlightComponent.hpp"

void registerHighlightComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::HighlightComponent
	>(em);
}
