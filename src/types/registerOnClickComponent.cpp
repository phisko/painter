#include "helpers/registerTypeHelper.hpp"
#include "data/OnClickComponent.hpp"

void registerOnClickComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::OnClickComponent
	>(em);
}
