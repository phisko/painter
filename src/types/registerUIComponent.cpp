#include "helpers/registerTypeHelper.hpp"
#include "data/UIComponent.hpp"

void registerUIComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		UIComponent
	>(em);
}
