// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/sign.hpp"
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/input/data/handler.hpp"
#include "kengine/model/helpers/has.hpp"
#include "kengine/model/helpers/try_get.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/pathfinding/data/nav_mesh.hpp"
#include "kengine/pathfinding/data/navigation.hpp"
#include "kengine/pathfinding/functions/get_path.hpp"
#include "kengine/physics/data/inertia.hpp"
#include "kengine/physics/kinematic/data/kinematic.hpp"
#include "kengine/render/functions/get_position_in_pixel.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

#ifndef KENGINE_NDEBUG
#include "kengine/render/data/debug_graphics.hpp"
#endif

#include "config.hpp"

namespace character_movement {
	struct system {
		entt::registry & r;
		const config * cfg = nullptr;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::main_loop::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::input::handler>() = {
				.on_mouse_button = click
			};

			e.emplace<kengine::core::name>("Character/Movement");
			e.emplace<kengine::config::configurable>();
			cfg = &e.emplace<config>();
		}

		static void click(entt::handle window, int button, const putils::point3f & screen_coordinates, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed || button != GLFW_MOUSE_BUTTON_RIGHT)
				return;

			auto & r = *window.registry();

			for (const auto & [e, get_position_in_pixel] : r.view<kengine::render::get_position_in_pixel>().each()) {
				const auto pos = get_position_in_pixel(window, screen_coordinates);
				if (!pos)
					continue;
				for (const auto & [e, navigation] : r.view<kengine::pathfinding::navigation>().each())
					navigation.destination = *pos;
			}
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			set_environments();
			update_orientations();
		}

		void set_environments() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [e, navigation] : r.view<kengine::pathfinding::navigation>().each()) {
				if (navigation.environment != entt::null)
					continue;
				for (const auto & [e, instance] : r.view<kengine::model::instance>().each())
					if (kengine::model::has<kengine::pathfinding::nav_mesh>(r, instance)) {
						navigation.environment = e;
						break;
					}
			}
		}

		void update_orientations() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, transform, inertia, navigation] : r.view<kengine::core::transform, kengine::physics::inertia, kengine::pathfinding::navigation>().each()) {
				if (navigation.environment == entt::null)
					continue;

				if (putils::get_length_squared(inertia.movement) < .1f)
					continue;

				r.get_or_emplace<kengine::physics::kinematic::kinematic>(e);

				const auto yaw_to = putils::get_yaw_from_normalized_direction(inertia.movement);
				const auto yaw_delta = putils::constrain_angle(putils::constrain_angle(yaw_to) - transform.yaw);
				if (std::abs(yaw_delta) > cfg->facing_strictness)
					inertia.yaw = cfg->turn_speed * putils::sign(yaw_delta);
				else
					inertia.yaw = 0.f;

				const auto pitch_to = putils::get_pitch_from_normalized_direction(inertia.movement);
				const auto pitch_delta = putils::constrain_angle(putils::constrain_angle(pitch_to) - transform.pitch);
				if (std::abs(pitch_delta) > cfg->facing_strictness)
					inertia.pitch = cfg->turn_speed * putils::sign(pitch_delta);
				else
					inertia.pitch = 0.f;

				const auto name = r.try_get<kengine::core::name>(e);
				debug(e, transform.bounding_box.position, navigation);
			}
		}

		struct debug_path {
			putils::point3f destination{ 0.f, 0.f, 0.f };
			kengine::pathfinding::get_path_impl::path path;
		};

		void debug(entt::entity e, const putils::point3f & pos, const kengine::pathfinding::navigation & navigation) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto obj = navigation.environment;
			const auto get_path = kengine::model::try_get<kengine::pathfinding::get_path>({ r, obj });
			if (!get_path)
				return;

			auto & path = r.get_or_emplace<debug_path>(e);
			if (path.destination != navigation.destination) {
				path.destination = navigation.destination;
				path.path = get_path->call({ r, obj }, pos, navigation.destination);
			}

			auto & debug = r.emplace_or_replace<kengine::render::debug_graphics>(e);

			bool first = true;
			putils::point3f last_step;
			for (auto step : path.path) {
				step.y += 1.f;

				kengine::render::debug_graphics::element debug_element;
				{
					debug_element.type = kengine::render::debug_graphics::element_type::sphere;
					debug_element.sphere.radius = .1f;
					debug_element.pos = step;
					debug_element.relative_to = kengine::render::debug_graphics::reference_space::world;
				};
				debug.elements.emplace_back(std::move(debug_element));

				if (!first) {
					kengine::render::debug_graphics::element debug_element;
					{
						debug_element.type = kengine::render::debug_graphics::element_type::line;
						debug_element.line.end = last_step;
						debug_element.pos = step;
						debug_element.relative_to = kengine::render::debug_graphics::reference_space::world;
					}
					debug.elements.emplace_back(std::move(debug_element));
				}

				first = false;
				last_step = step;
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system, system::debug_path);
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	character_movement::add_system(r);
}