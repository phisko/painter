#include "EntityHighlightSystem.hpp"

#include "EntityManager.hpp"
#include "Export.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/HighlightComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/SpriteComponent.hpp"

#include "packets/EntityInPixel.hpp"

struct HoveredComponent {};

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new EntityHighlightSystem(em);
}

static putils::NormalizedColor SELECTED_COLOR;
static float SELECTED_INTENSITY = 2.f;

static putils::NormalizedColor HOVERED_COLOR;
static float HOVERED_INTENSITY = 1.f;

EntityHighlightSystem::EntityHighlightSystem(kengine::EntityManager & em) : System(em), _em(em) {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Selected] Color", &SELECTED_COLOR); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Selected] Intensity", &SELECTED_INTENSITY); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Hovered] Color", &HOVERED_COLOR); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Hovered] Intensity", &HOVERED_INTENSITY); };

	_em += [this](kengine::Entity & e) {
		kengine::InputComponent input;

		input.onMouseButton = [this](kengine::Entity::ID window, int button, const putils::Point2f & coords, bool pressed) {
			if (!pressed)
				return;
			click(window, coords);
		};

		input.onMouseMove = [this](kengine::Entity::ID window, const putils::Point2f & coords, const putils::Point2f & rel) {
			hover(window, coords);
		};

		e += input;
	};
}

void EntityHighlightSystem::click(kengine::Entity::ID window, const putils::Point2f & coords) noexcept {
	kengine::Entity::ID id = kengine::Entity::INVALID_ID;
	send(kengine::packets::GetEntityInPixel{ window, coords, id });

	if (id == kengine::Entity::INVALID_ID)
		return;

	auto & e = _em.getEntity(id);
	if (e.has<kengine::SelectedComponent>())
		e.detach<kengine::SelectedComponent>();
	else {
		e += kengine::SelectedComponent{};
		e += kengine::HighlightComponent{ .color = SELECTED_COLOR,.intensity = SELECTED_INTENSITY };
	}
}

void EntityHighlightSystem::hover(kengine::Entity::ID window, const putils::Point2f & coords) noexcept {
	static kengine::Entity::ID previous = kengine::Entity::INVALID_ID;

	kengine::Entity::ID hovered = kengine::Entity::INVALID_ID;
	send(kengine::packets::GetEntityInPixel{ window, coords, hovered });

	if (hovered == previous)
		return;

	if (previous != kengine::Entity::INVALID_ID) {
		auto & e = _em.getEntity(previous);
		if (e.has<HoveredComponent>())
			e.detach<HoveredComponent>();

		previous = kengine::Entity::INVALID_ID;
	}

	if (hovered != kengine::Entity::INVALID_ID) {
		auto & e = _em.getEntity(hovered);
		if (!e.has<kengine::SelectedComponent>()) {
			e += HoveredComponent{};
			e += kengine::HighlightComponent{ .color = HOVERED_COLOR,.intensity = HOVERED_INTENSITY };
		}

		previous = hovered;
	}
}

void EntityHighlightSystem::execute() {
	for (auto &[e, highlight] : _em.getEntities<kengine::HighlightComponent>())
		if (!e.has<kengine::SelectedComponent>() && !e.has<HoveredComponent>())
			e.detach<kengine::HighlightComponent>();

	for (auto &[e, selected] : _em.getEntities<kengine::SelectedComponent>())
		if (!e.has<kengine::HighlightComponent>())
			e += kengine::HighlightComponent{ .color = SELECTED_COLOR, .intensity = SELECTED_INTENSITY };
}
