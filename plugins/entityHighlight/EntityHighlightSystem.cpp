#include <GLFW/glfw3.h>

#include "kengine.hpp"
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

static putils::NormalizedColor SELECTED_COLOR;
static float SELECTED_INTENSITY = 2.f;

static putils::NormalizedColor HOVERED_COLOR;
static float HOVERED_INTENSITY = 1.f;

EXPORT void loadKenginePlugin(void * state) noexcept {
	struct impl {
		static void init() noexcept {
			kengine::entities += [](kengine::Entity & e) noexcept {
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
				input.onMouseButton = [](kengine::EntityID window, int button, const putils::Point2f & coords, bool pressed) noexcept {
					if (!pressed || button != GLFW_MOUSE_BUTTON_LEFT)
						return;
					click(window, coords);
				};
				input.onMouseMove = [](kengine::EntityID window, const putils::Point2f & coords, const putils::Point2f & rel) noexcept {
					hover(window, coords);
				};

				e += input;
			};
		}

		static void execute(float deltaTime) {
			using kengine::no;

			for (auto [e, highlight, noSelected, noHovered] : kengine::entities.with<kengine::HighlightComponent, no<kengine::SelectedComponent>, no<HoveredComponent>>())
				e.detach<kengine::HighlightComponent>();

			for (auto [e, selected, notHighlighted] : kengine::entities.with<kengine::SelectedComponent, no<kengine::HighlightComponent>>())
				e += kengine::HighlightComponent{ .color = SELECTED_COLOR, .intensity = SELECTED_INTENSITY };
		}

		static void click(kengine::EntityID window, const putils::Point2f & coords) noexcept {
			kengine::EntityID id = kengine::INVALID_ID;
			for (const auto & [e, func] : kengine::entities.with<kengine::functions::GetEntityInPixel>()) {
				id = func(window, coords);
				if (id != kengine::INVALID_ID)
					break;
			}

			if (id == kengine::INVALID_ID)
				return;

			auto e = kengine::entities[id];
			if (e.has<kengine::SelectedComponent>())
				e.detach<kengine::SelectedComponent>();
			else {
				e += kengine::SelectedComponent{};
				e += kengine::HighlightComponent{ .color = SELECTED_COLOR,.intensity = SELECTED_INTENSITY };
			}
		}

		static void hover(kengine::EntityID window, const putils::Point2f & coords) noexcept {
			static kengine::EntityID previous = kengine::INVALID_ID;

			kengine::EntityID hovered = kengine::INVALID_ID;
			for (const auto & [e, func] : kengine::entities.with<kengine::functions::GetEntityInPixel>()) {
				hovered = func(window, coords);
				if (hovered != kengine::INVALID_ID)
					break;
			}

			if (hovered == previous)
				return;

			if (previous != kengine::INVALID_ID) {
				auto e = kengine::entities[previous];
				if (e.has<HoveredComponent>())
					e.detach<HoveredComponent>();

				previous = kengine::INVALID_ID;
			}

			if (hovered != kengine::INVALID_ID) {
				auto e = kengine::entities[hovered];
				if (!e.has<kengine::SelectedComponent>()) {
					e += HoveredComponent{};
					e += kengine::HighlightComponent{ .color = HOVERED_COLOR,.intensity = HOVERED_INTENSITY };
				}

				previous = hovered;
			}
		}
	};

	kengine::pluginHelper::initPlugin(state);
	impl::init();
}
