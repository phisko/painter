#pragma once

#include "System.hpp"

class UISystem : public kengine::System<UISystem> {
public:
	UISystem(kengine::EntityManager & em);

	void execute() override;
	void onLoad(const char *) noexcept override;

private:
	kengine::EntityManager & _em;
};