#pragma once

#include "System.hpp"

class EntityHighlightSystem : public kengine::System<EntityHighlightSystem> {
public:
	EntityHighlightSystem(kengine::EntityManager & em);

	void execute() override;
	void onLoad(const char *) noexcept override;

private:
	void click(unsigned int x, unsigned int y) noexcept;
	void hover(unsigned int x, unsigned int y) noexcept;

private:
	kengine::EntityManager & _em;
};