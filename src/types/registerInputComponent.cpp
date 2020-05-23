#include "helpers/registerTypeHelper.hpp"
#include "data/InputComponent.hpp"

void registerInputComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::InputComponent
	>(em);

	kengine::registerTypes<
		putils::Point2f
	>(em);
}
