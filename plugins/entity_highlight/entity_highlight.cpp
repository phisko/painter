#include "Export.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/input.hpp"
#include "kengine/data/highlight.hpp"
#include "kengine/data/selected.hpp"

// kengine functions
#include "kengine/functions/get_entity_in_pixel.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace data {
	struct hovered {};
}

namespace systems {
	struct entity_highlight {
		entt::registry & r;

		struct {
			putils::normalized_color selected_color;
			float selected_intensity = 2.f;

			putils::normalized_color hovered_color;
			float hovered_intensity = 1.f;
		} adjustables;

		entity_highlight(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::data::adjustable>() = {
				"Highlight",
				{
					{ "Selected color", &adjustables.selected_color },
					{ "Selected intensity", &adjustables.selected_intensity },
					{ "Hovered color", &adjustables.hovered_color },
					{ "Hovered intensity", &adjustables.hovered_intensity },
				}
			};

			kengine::data::input input;
			input.on_mouse_button = [this](entt::handle window, int button, const putils::point2f & coords, bool pressed) noexcept {
				if (!pressed || button != GLFW_MOUSE_BUTTON_LEFT)
					return;
				click(window, coords);
			};
			input.on_mouse_move = [this](entt::handle window, const putils::point2f & coords, const putils::point2f & rel) noexcept {
				hover(window, coords);
			};

			e.emplace<kengine::data::input>(input);

			r.on_construct<kengine::data::selected>().connect<&entity_highlight::on_construct_selected>(this);
			r.on_destroy<kengine::data::selected>().connect<&entity_highlight::on_destroy_selected>(this);
			r.on_construct<data::hovered>().connect<&entity_highlight::on_construct_hovered>(this);
			r.on_destroy<data::hovered>().connect<&entity_highlight::on_destroy_hovered>(this);
		}

		void on_construct_selected(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			r.emplace_or_replace<kengine::data::highlight>(
				e,
				kengine::data::highlight{
					.color = adjustables.selected_color,
					.intensity = adjustables.selected_intensity,
				}
			);
		}

		void on_destroy_selected(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			if (!r.any_of<data::hovered>(e))
				r.erase<kengine::data::highlight>(e);
		}

		void on_construct_hovered(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			// Don't replace the existing one, leave priority to the data::selected
			(void)r.get_or_emplace<kengine::data::highlight>(
				e,
				kengine::data::highlight{
					.color = adjustables.hovered_color,
					.intensity = adjustables.hovered_intensity,
				}
			);
		}

		void on_destroy_hovered(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			if (!r.any_of<kengine::data::selected>(e))
				r.erase<kengine::data::highlight>(e);
		}

		void click(entt::handle window, const putils::point2f & coords) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & r = *window.registry();

			entt::entity entity_in_pixel = entt::null;
			for (const auto & [e, func] : r.view<kengine::functions::get_entity_in_pixel>().each()) {
				entity_in_pixel = func(window, coords);
				if (entity_in_pixel != entt::null)
					break;
			}

			if (entity_in_pixel == entt::null)
				return;

			if (r.all_of<kengine::data::selected>(entity_in_pixel))
				r.remove<kengine::data::selected>(entity_in_pixel);
			else {
				r.emplace<kengine::data::selected>(entity_in_pixel);
				r.emplace_or_replace<kengine::data::highlight>(
					entity_in_pixel,
					kengine::data::highlight{
						.color = adjustables.selected_color,
						.intensity = adjustables.selected_intensity,
					}
				);
			}
		}

		entt::entity previous = entt::null;
		void hover(entt::handle window, const putils::point2f & coords) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & r = *window.registry();

			entt::entity hovered = entt::null;
			for (const auto & [e, func] : r.view<kengine::functions::get_entity_in_pixel>().each()) {
				hovered = func(window, coords);
				if (hovered != entt::null)
					break;
			}

			if (hovered == previous)
				return;

			if (previous != entt::null) {
				r.remove<data::hovered>(previous);
				previous = entt::null;
			}

			if (hovered != entt::null) {
				if (!r.all_of<kengine::data::selected>(hovered))
					r.emplace<data::hovered>(hovered);

				previous = hovered;
			}
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::entity_highlight>(e);
}
