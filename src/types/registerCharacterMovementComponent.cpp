#include "registerTypes.hpp"
#include "data/CharacterMovementComponent.hpp"

void registerCharacterMovementComponent(kengine::EntityManager & em) {
	registerComponents<
		CharacterMovementComponent
	>(em);
}