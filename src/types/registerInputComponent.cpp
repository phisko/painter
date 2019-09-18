#include "registerTypes.hpp"
#include "components/InputComponent.hpp"

void registerInputComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::InputComponent
	>(em);
}
