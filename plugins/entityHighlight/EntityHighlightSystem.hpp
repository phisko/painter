#pragma once

#include "System.hpp"
#include "Point.hpp"

class EntityHighlightSystem : public kengine::System<EntityHighlightSystem> {
public:
	EntityHighlightSystem(kengine::EntityManager & em);

	void execute() override;
	void onLoad(const char *) noexcept override;

private:
	void click(const putils::Point2f & coords) noexcept;
	void hover(const putils::Point2f & coords) noexcept;

private:
	kengine::EntityManager & _em;
};