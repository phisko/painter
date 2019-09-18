#include "registerTypes.hpp"
#include "components/UIComponent.hpp"

void registerUIComponent(kengine::EntityManager & em) {
	registerComponents<
		UIComponent
	>(em);
}
