#include "helpers/registerTypeHelper.hpp"
#include "data/GraphicsComponent.hpp"

void registerGraphicsComponent() {
	kengine::registerComponents<
		kengine::GraphicsComponent
	>();
}
