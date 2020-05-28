#include <GLFW/glfw3.h>

#include "EntityManager.hpp"
#include "Export.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/HighlightComponent.hpp"
#include "data/SelectedComponent.hpp"
#include "data/SpriteComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/GetEntityInPixel.hpp"

#include "helpers/pluginHelper.hpp"

struct HoveredComponent {};

static kengine::EntityManager * g_em;

static putils::NormalizedColor SELECTED_COLOR;
static float SELECTED_INTENSITY = 2.f;

static putils::NormalizedColor HOVERED_COLOR;
static float HOVERED_INTENSITY = 1.f;

#pragma region declarations
static void execute(float deltaTime);
static void click(kengine::Entity::ID window, const putils::Point2f & coords);
static void hover(kengine::Entity::ID window, const putils::Point2f & coords);
#pragma endregion
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::pluginHelper::initPlugin(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };

		e += kengine::AdjustableComponent{
			"Highlight", {
				{ "Selected color", &SELECTED_COLOR },
				{ "Selected intensity", &SELECTED_INTENSITY },
				{ "Hovered color", &HOVERED_COLOR },
				{ "Hovered intensity", &HOVERED_INTENSITY }
			}
		};

		kengine::InputComponent input;
		input.onMouseButton = [](kengine::Entity::ID window, int button, const putils::Point2f & coords, bool pressed) {
			if (!pressed || button != GLFW_MOUSE_BUTTON_LEFT)
				return;
			click(window, coords);
		};
		input.onMouseMove = [](kengine::Entity::ID window, const putils::Point2f & coords, const putils::Point2f & rel) {
			hover(window, coords);
		};

		e += input;
	};
}

static void execute(float deltaTime) {
	using kengine::no;

	for (auto &[e, highlight, noSelected, noHovered] : g_em->getEntities<kengine::HighlightComponent, no<kengine::SelectedComponent>, no<HoveredComponent>>())
		e.detach<kengine::HighlightComponent>();

	for (auto &[e, selected, notHighlighted] : g_em->getEntities<kengine::SelectedComponent, no<kengine::HighlightComponent>>())
		e += kengine::HighlightComponent{ .color = SELECTED_COLOR, .intensity = SELECTED_INTENSITY };
}

static void click(kengine::Entity::ID window, const putils::Point2f & coords) {
	kengine::Entity::ID id = kengine::Entity::INVALID_ID;
	for (const auto & [e, func] : g_em->getEntities<kengine::functions::GetEntityInPixel>()) {
		id = func(window, coords);
		if (id != kengine::Entity::INVALID_ID)
			break;
	}

	if (id == kengine::Entity::INVALID_ID)
		return;

	auto & e = g_em->getEntity(id);
	if (e.has<kengine::SelectedComponent>())
		e.detach<kengine::SelectedComponent>();
	else {
		e += kengine::SelectedComponent{};
		e += kengine::HighlightComponent{ .color = SELECTED_COLOR,.intensity = SELECTED_INTENSITY };
	}
}

static void hover(kengine::Entity::ID window, const putils::Point2f & coords) {
	static kengine::Entity::ID previous = kengine::Entity::INVALID_ID;

	kengine::Entity::ID hovered = kengine::Entity::INVALID_ID;
	for (const auto & [e, func] : g_em->getEntities<kengine::functions::GetEntityInPixel>()) {
		hovered = func(window, coords);
		if (hovered != kengine::Entity::INVALID_ID)
			break;
	}

	if (hovered == previous)
		return;

	if (previous != kengine::Entity::INVALID_ID) {
		auto & e = g_em->getEntity(previous);
		if (e.has<HoveredComponent>())
			e.detach<HoveredComponent>();

		previous = kengine::Entity::INVALID_ID;
	}

	if (hovered != kengine::Entity::INVALID_ID) {
		auto & e = g_em->getEntity(hovered);
		if (!e.has<kengine::SelectedComponent>()) {
			e += HoveredComponent{};
			e += kengine::HighlightComponent{ .color = HOVERED_COLOR,.intensity = HOVERED_INTENSITY };
		}

		previous = hovered;
	}
}
