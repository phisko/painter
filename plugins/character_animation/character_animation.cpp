// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/sign.hpp"
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/meta/helpers/register_everything.hpp"
#include "kengine/physics/data/inertia.hpp"
#include "kengine/physics/kinematic/data/kinematic.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

#include "config.hpp"

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

namespace character_animation {
	struct system {
		entt::registry & r;
		const config * cfg = nullptr;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			kengine::meta::register_everything<data::wobble>(r);

			e.emplace<kengine::main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<kengine::core::name>("Character/Wobble");
			e.emplace<kengine::config::configurable>();
			cfg = &e.emplace<config>();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			return;

			for (auto [e, transform, inertia] : r.view<kengine::core::transform, kengine::physics::inertia, kengine::physics::kinematic::kinematic>().each()) {
				auto & wobble = r.emplace<data::wobble>(e);

				const auto moving = putils::get_length_squared(inertia.movement) > 0.f;
				if (!moving)
					wobble.target_roll = 0.f;
				else if (wobble.target_roll == 0.f)
					wobble.target_roll = cfg->max_roll;

				const auto delta_roll = wobble.target_roll - transform.roll;
				inertia.roll = cfg->wobble_speed * putils::sign(delta_roll);
				if (std::abs(delta_roll) < .05f) {
					if (moving)
						wobble.target_roll = wobble.target_roll <= 0.f ? cfg->max_roll : -cfg->max_roll;
					else
						inertia.roll = 0.f;
				}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system, data::wobble);
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	character_animation::add_system(r);
}