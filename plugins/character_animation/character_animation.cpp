#include "Export.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/sign.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/kinematic.hpp"
#include "kengine/data/physics.hpp"
#include "kengine/data/transform.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/register_type_helper.hpp"

namespace data {
	struct wobble {
		float target_roll = 0.f;
	};
}

#define refltype data::wobble
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

namespace systems {
	struct character_animation {
		entt::registry & r;

		struct {
			float wobble_speed = 1.f;
			float max_roll = 1.f;
		} adjustables;

		character_animation(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			kengine::register_components<data::wobble>(r);

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::data::adjustable>() = {
				"Character/Wobble",
				{
					{ "Speed", &adjustables.wobble_speed },
					{ "Max roll", &adjustables.max_roll },
				}
			};
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			return;

			for (auto [e, transform, physics] : r.view<kengine::data::transform, kengine::data::physics, kengine::data::kinematic>().each()) {
				auto & wobble = r.emplace<data::wobble>(e);

				const auto moving = putils::get_length_squared(physics.movement) > 0.f;
				if (!moving)
					wobble.target_roll = 0.f;
				else if (wobble.target_roll == 0.f)
					wobble.target_roll = adjustables.max_roll;

				const auto delta_roll = wobble.target_roll - transform.roll;
				physics.roll = adjustables.wobble_speed * putils::sign(delta_roll);
				if (std::abs(delta_roll) < .05f) {
					if (moving)
						wobble.target_roll = wobble.target_roll <= 0.f ? adjustables.max_roll : -adjustables.max_roll;
					else
						physics.roll = 0.f;
				}
			}
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::character_animation>(e);
}