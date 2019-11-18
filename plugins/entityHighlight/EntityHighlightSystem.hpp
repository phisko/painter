#pragma once

#include "System.hpp"
#include "Point.hpp"

class EntityHighlightSystem : public kengine::System<EntityHighlightSystem> {
public:
	EntityHighlightSystem(kengine::EntityManager & em);
	void execute() override;

private:
	void click(kengine::Entity::ID window, const putils::Point2f & coords) noexcept;
	void hover(kengine::Entity::ID window, const putils::Point2f & coords) noexcept;

private:
	kengine::EntityManager & _em;
};