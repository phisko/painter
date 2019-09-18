#include "registerTypes.hpp"
#include "components/OnClickComponent.hpp"

void registerOnClickComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::OnClickComponent
	>(em);
}
