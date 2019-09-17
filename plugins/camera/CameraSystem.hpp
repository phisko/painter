#pragma once

#include "System.hpp"
#include "components/CameraComponent.hpp"

class CameraSystem : public kengine::System<CameraSystem> {
public:
	CameraSystem(kengine::EntityManager & em);
	void execute() override;
	void onLoad(const char *) noexcept override;

private:
	kengine::EntityManager & _em;
};
