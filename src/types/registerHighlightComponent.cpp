#include "helpers/registerTypeHelper.hpp"
#include "data/HighlightComponent.hpp"

void registerHighlightComponent() {
	kengine::registerComponents<
		kengine::HighlightComponent
	>();
}
