#pragma once

#include "System.hpp"

class RotateSunSystem : public kengine::System<RotateSunSystem> {
public:
	RotateSunSystem(kengine::EntityManager & em);

	void execute() override;
	void onLoad(const char *) noexcept override;

private:
	kengine::EntityManager & _em;
};