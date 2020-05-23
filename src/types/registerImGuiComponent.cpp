#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiComponent.hpp"
#include "data/ImGuiToolComponent.hpp"

void registerImGuiComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::ImGuiComponent,
		kengine::ImGuiToolComponent
	>(em);
}
