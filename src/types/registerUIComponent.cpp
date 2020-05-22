#include "helpers/RegisterTypeHelper.hpp"
#include "data/UIComponent.hpp"

void registerUIComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		UIComponent
	>(em);
}
