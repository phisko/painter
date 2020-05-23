#include "helpers/registerTypeHelper.hpp"
#include "data/CharacterMovementComponent.hpp"

void registerCharacterMovementComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		CharacterMovementComponent
	>(em);
}
