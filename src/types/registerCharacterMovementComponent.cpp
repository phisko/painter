#include "registerTypes.hpp"
#include "components/CharacterMovementComponent.hpp"

void registerCharacterMovementComponent(kengine::EntityManager & em) {
	registerComponents<
		CharacterMovementComponent
	>(em);
}