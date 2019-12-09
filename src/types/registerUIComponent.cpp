#include "registerTypes.hpp"
#include "data/UIComponent.hpp"

void registerUIComponent(kengine::EntityManager & em) {
	registerComponents<
		UIComponent
	>(em);
}
