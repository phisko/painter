#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiToolComponent.hpp"

void registerImGuiComponent() {
	kengine::registerComponents<
		kengine::ImGuiToolComponent
	>();
}
