#include "registerTypes.hpp"
#include "data/OnClickComponent.hpp"

void registerOnClickComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::OnClickComponent
	>(em);
}
