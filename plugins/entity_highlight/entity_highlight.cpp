// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// putils
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/selected.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/input/data/handler.hpp"
#include "kengine/render/data/highlight.hpp"
#include "kengine/render/functions/get_entity_in_pixel.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

#include "config.hpp"

namespace data {
	struct hovered {};
}

namespace entity_highlight {
	struct system {
		entt::registry & r;
		const config * cfg = nullptr;

		const entt::scoped_connection connections[4] = {
			r.on_construct<kengine::core::selected>().connect<&system::on_construct_selected>(this),
			r.on_destroy<kengine::core::selected>().connect<&system::on_destroy_selected>(this),
			r.on_construct<data::hovered>().connect<&system::on_construct_hovered>(this),
			r.on_destroy<data::hovered>().connect<&system::on_destroy_hovered>(this),
		};

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::core::name>("Highlight");
			e.emplace<kengine::config::configurable>();
			cfg = & e.emplace<config>();

			kengine::input::handler input;
			input.on_mouse_button = [this](entt::handle window, int button, const putils::point2f & coords, bool pressed) noexcept {
				if (!pressed || button != GLFW_MOUSE_BUTTON_LEFT)
					return;
				click(window, coords);
			};
			input.on_mouse_move = [this](entt::handle window, const putils::point2f & coords, const putils::point2f & rel) noexcept {
				hover(window, coords);
			};

			e.emplace<kengine::input::handler>(input);
		}

		void on_construct_selected(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			r.emplace_or_replace<kengine::render::highlight>(
				e,
				kengine::render::highlight{
					.color = cfg->selected_color,
					.intensity = cfg->selected_intensity,
				}
			);
		}

		void on_destroy_selected(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			if (!r.any_of<data::hovered>(e))
				r.remove<kengine::render::highlight>(e);
		}

		void on_construct_hovered(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			// Don't replace the existing one, leave priority to the core::selected
			(void)r.get_or_emplace<kengine::render::highlight>(
				e,
				kengine::render::highlight{
					.color = cfg->hovered_color,
					.intensity = cfg->hovered_intensity,
				}
			);
		}

		void on_destroy_hovered(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			if (!r.any_of<kengine::core::selected>(e))
				r.remove<kengine::render::highlight>(e);
		}

		void click(entt::handle window, const putils::point2f & coords) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & r = *window.registry();

			entt::entity entity_in_pixel = entt::null;
			for (const auto & [e, func] : r.view<kengine::render::get_entity_in_pixel>().each()) {
				entity_in_pixel = func(window, coords);
				if (entity_in_pixel != entt::null)
					break;
			}

			if (entity_in_pixel == entt::null)
				return;

			if (r.all_of<kengine::core::selected>(entity_in_pixel))
				r.remove<kengine::core::selected>(entity_in_pixel);
			else {
				r.emplace<kengine::core::selected>(entity_in_pixel);
				r.emplace_or_replace<kengine::render::highlight>(
					entity_in_pixel,
					kengine::render::highlight{
						.color = cfg->selected_color,
						.intensity = cfg->selected_intensity,
					}
				);
			}
		}

		entt::entity previous = entt::null;
		void hover(entt::handle window, const putils::point2f & coords) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & r = *window.registry();

			entt::entity hovered = entt::null;
			for (const auto & [e, func] : r.view<kengine::render::get_entity_in_pixel>().each()) {
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
				if (!r.all_of<kengine::core::selected>(hovered))
					r.emplace<data::hovered>(hovered);

				previous = hovered;
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system, data::hovered);
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	entity_highlight::add_system(r);
}
