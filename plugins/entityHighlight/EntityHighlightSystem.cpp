#include "EntityHighlightSystem.hpp"

#include "EntityManager.hpp"
#include "Export.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/HighlightComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/NameComponent.hpp"

#include "packets/EntityInPixel.hpp"

struct HoveredComponent {};

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new EntityHighlightSystem(em);
}

EntityHighlightSystem::EntityHighlightSystem(kengine::EntityManager & em) : System(em), _em(em) {
	onLoad("");
}

static putils::NormalizedColor SELECTED_COLOR;
static float SELECTED_INTENSITY = 2.f;

static putils::NormalizedColor HOVERED_COLOR;
static float HOVERED_INTENSITY = 1.f;

void EntityHighlightSystem::onLoad(const char *) noexcept {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Selected] Color", &SELECTED_COLOR); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Selected] Intensity", &SELECTED_INTENSITY); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Hovered] Color", &HOVERED_COLOR); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Highlight/Hovered] Intensity", &HOVERED_INTENSITY); };

	_em += [this](kengine::Entity & e) {
		kengine::InputComponent input;

		input.onMouseButton = [this](int button, float x, float y, bool pressed) {
			if (!pressed)
				return;
			click((unsigned int)x, (unsigned int)y);
		};

		input.onMouseMove = [this](float x, float y, float xrel, float yrel) {
			hover((unsigned int)x, (unsigned int)y);
		};

		e += input;
		e += kengine::NameComponent{ "Highlight controller" };
	};
}

void EntityHighlightSystem::click(unsigned int x, unsigned int y) noexcept {
	kengine::Entity::ID id = kengine::Entity::INVALID_ID;
	send(kengine::packets::GetEntityInPixel{ { x, y }, id });

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

void EntityHighlightSystem::hover(unsigned int x, unsigned int y) noexcept {
	static kengine::Entity::ID previous = kengine::Entity::INVALID_ID;

	kengine::Entity::ID hovered = kengine::Entity::INVALID_ID;
	send(kengine::packets::GetEntityInPixel{ { x, y }, hovered });

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
