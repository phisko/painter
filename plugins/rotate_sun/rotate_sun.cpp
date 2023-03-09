// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/render/data/light.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

#include "config.hpp"

namespace rotate_sun {
	struct system {
		entt::registry & r;
		const config * cfg = nullptr;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<kengine::core::name>("Sun");
			e.emplace<kengine::config::configurable>();
			cfg = &e.emplace<config>();
		}

		float angle = 0.f;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, light] : r.view<kengine::render::dir_light>().each()) {
				angle += delta_time * cfg->sun_rotation;
				if (angle > putils::pi * 2.f)
					angle -= putils::pi * 2.f;
				light.direction = { std::cos(angle), -1.f, std::sin(angle) };
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system);
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	rotate_sun::add_system(r);
}
