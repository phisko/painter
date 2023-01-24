#include "Export.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/sign.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/input.hpp"
#include "kengine/data/kinematic.hpp"
#include "kengine/data/name.hpp"
#include "kengine/data/nav_mesh.hpp"
#include "kengine/data/pathfinding.hpp"
#include "kengine/data/physics.hpp"
#include "kengine/data/transform.hpp"

#ifndef KENGINE_NDEBUG
#include "kengine/data/debug_graphics.hpp"
#endif

// kengine functions
#include "kengine/functions/execute.hpp"
#include "kengine/functions/get_position_in_pixel.hpp"

// kengine helpers
#include "kengine/helpers/instance_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace systems {
	struct character_movement {
		entt::registry & r;

		struct {
			float facing_strictness = 0.05f;
			float turn_speed = putils::pi;
		} adjustables;

		character_movement(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::data::adjustable>() = {
				"Character/Movement",
				{
					{ "Facing strictness", &adjustables.facing_strictness },
					{ "Turn speed", &adjustables.turn_speed },
				}
			};

			e.emplace<kengine::data::input>() = {
				.on_mouse_button = click
			};
		}

		static void click(entt::handle window, int button, const putils::point3f & screen_coordinates, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed || button != GLFW_MOUSE_BUTTON_RIGHT)
				return;

			auto & r = *window.registry();

			for (const auto & [e, get_position_in_pixel] : r.view<kengine::functions::get_position_in_pixel>().each()) {
				const auto pos = get_position_in_pixel(window, screen_coordinates);
				if (!pos)
					continue;
				for (const auto & [e, pathfinding] : r.view<kengine::data::pathfinding>().each())
					pathfinding.destination = *pos;
			}
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			set_environments();
			update_orientations();
		}

		void set_environments() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [e, pathfinding] : r.view<kengine::data::pathfinding>().each()) {
				if (pathfinding.environment != entt::null)
					continue;
				for (const auto & [e, instance] : r.view<kengine::data::instance>().each())
					if (kengine::instance_helper::model_has<kengine::data::nav_mesh>(r, instance)) {
						pathfinding.environment = e;
						break;
					}
			}
		}

		void update_orientations() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, transform, physics, pathfinding] : r.view<kengine::data::transform, kengine::data::physics, kengine::data::pathfinding>().each()) {
				if (pathfinding.environment == entt::null)
					continue;

				r.get_or_emplace<kengine::data::kinematic>(e);

				const auto yaw_to = putils::get_yaw_from_normalized_direction(physics.movement);
				const auto yaw_delta = putils::constrain_angle(putils::constrain_angle(yaw_to) - transform.yaw);
				if (std::abs(yaw_delta) > adjustables.facing_strictness)
					physics.yaw = adjustables.turn_speed * putils::sign(yaw_delta);
				else
					physics.yaw = 0.f;

				const auto pitch_to = putils::get_pitch_from_normalized_direction(physics.movement);
				const auto pitch_delta = putils::constrain_angle(putils::constrain_angle(pitch_to) - transform.pitch);
				if (std::abs(pitch_delta) > adjustables.facing_strictness)
					physics.pitch = adjustables.turn_speed * putils::sign(pitch_delta);
				else
					physics.pitch = 0.f;

				const auto name = r.try_get<kengine::data::name>(e);
				debug(e, transform.bounding_box.position, pathfinding);
			}
		}

		void debug(entt::entity e, const putils::point3f & pos, const kengine::data::pathfinding & pathfinding) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto obj = pathfinding.environment;
			const auto & get_path = kengine::instance_helper::get_model<kengine::functions::get_path>({ r, obj });
			const auto path = get_path({ r, obj }, pos, pathfinding.destination);

			auto & debug = r.emplace_or_replace<kengine::data::debug_graphics>(e);

			bool first = true;
			putils::point3f last_pos;
			for (auto pos : path) {
				pos.y += 1.f;

				kengine::data::debug_graphics::element debug_element;
				{
					debug_element.type = kengine::data::debug_graphics::element_type::sphere;
					debug_element.sphere.radius = .1f;
					debug_element.pos = pos;
					debug_element.relative_to = kengine::data::debug_graphics::reference_space::world;
				};
				debug.elements.emplace_back(std::move(debug_element));

				if (!first) {
					kengine::data::debug_graphics::element debug_element;
					{
						debug_element.type = kengine::data::debug_graphics::element_type::line;
						debug_element.line.end = last_pos;
						debug_element.pos = pos;
						debug_element.relative_to = kengine::data::debug_graphics::reference_space::world;
					}
					debug.elements.emplace_back(std::move(debug_element));
				}

				first = false;
				last_pos = pos;
			}
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::character_movement>(e);
}