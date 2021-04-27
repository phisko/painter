#include "helpers/registerTypeHelper.hpp"
#include "functions/OnClick.hpp"

void registerOnClickComponent() {
	kengine::registerComponents<
		kengine::functions::OnClick
	>();
}
