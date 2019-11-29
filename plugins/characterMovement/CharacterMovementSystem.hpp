#pragma once

#include "System.hpp"
#include "Point.hpp"

class CharacterMovementSystem : public kengine::System<CharacterMovementSystem> {
public:
	CharacterMovementSystem(kengine::EntityManager & em);
	void execute() override;

private:
	kengine::EntityManager & _em;
};