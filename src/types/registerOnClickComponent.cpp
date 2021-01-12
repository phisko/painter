#include "helpers/registerTypeHelper.hpp"
#include "data/OnClickComponent.hpp"

void registerOnClickComponent() {
	kengine::registerComponents<
		kengine::OnClickComponent
	>();
}
