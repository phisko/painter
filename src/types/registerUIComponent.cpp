#include "helpers/registerTypeHelper.hpp"
#include "data/UIComponent.hpp"

void registerUIComponent() {
	kengine::registerComponents<
		UIComponent
	>();
}
