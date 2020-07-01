#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiToolComponent.hpp"

void registerImGuiComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::ImGuiToolComponent
	>(em);
}
