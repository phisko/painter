#pragma once

#include "System.hpp"

#include "packets/Terminate.hpp"

class LoadingSystem : public kengine::System<LoadingSystem, kengine::packets::Terminate> {
public:
	LoadingSystem(kengine::EntityManager & em);

	void execute() override;
	void handle(kengine::packets::Terminate);

private:
	kengine::EntityManager & _em;
};