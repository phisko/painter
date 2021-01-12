#include "helpers/registerTypeHelper.hpp"
#include "data/InputComponent.hpp"

void registerInputComponent() {
	kengine::registerComponents<
		kengine::InputComponent
	>();

	kengine::registerTypes<
		putils::Point2f
	>();
}
