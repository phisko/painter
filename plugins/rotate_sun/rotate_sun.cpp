#include "Export.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/light.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace systems {
	struct rotate_sun {
		entt::registry & r;

		struct {
			float sun_rotation = .1f;
		} adjustables;

		rotate_sun(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::data::adjustable>() = {
				"world",
				{
					{ "Sun rotation speed", &adjustables.sun_rotation },
				}
			};
		}

		float angle = 0.f;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, light] : r.view<kengine::data::dir_light>().each()) {
				angle += delta_time * adjustables.sun_rotation;
				if (angle > putils::pi * 2.f)
					angle -= putils::pi * 2.f;
				light.direction = { std::cos(angle), -1.f, std::sin(angle) };
			}
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::rotate_sun>(e);
}
