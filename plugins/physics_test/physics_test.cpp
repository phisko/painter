// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/rand.hpp"
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/input/data/handler.hpp"
#include "kengine/model/data/instance.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/pathfinding/data/navigation.hpp"
#include "kengine/physics/data/inertia.hpp"
#include "kengine/render/data/drawable.hpp"
#include "kengine/render/data/no_shadow.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

#include "config.hpp"

namespace data {
	struct particle_timer {
		float lifetime = 0.f;
	};
}

namespace physics_test {
	struct system {
		entt::registry & r;

		size_t particle_count = 0;
		const config * cfg = nullptr;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::main_loop::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::input::handler>() = { .on_key = putils_forward_to_this(on_key) };

			e.emplace<kengine::core::name>("Physics/Debug");
			e.emplace<kengine::config::configurable>();
			cfg = &e.emplace<config>();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [e, timer] : r.view<data::particle_timer>().each()) {
				timer.lifetime += delta_time;
				if (timer.lifetime >= cfg->particle_duration) {
					r.destroy(e);
					--particle_count;
				}
			}

			if (!kengine::imgui::set_context(r))
				return;

			if (ImGui::Begin("Particles"))
				ImGui::Text("Count: %zu", particle_count);
			ImGui::End();
		}

		void on_key(entt::handle window, int keycode, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed || keycode != 'Y')
				return;

			for (auto [e, physics] : window.registry()->view<kengine::pathfinding::navigation>().each())
				for (int i = 0; i < cfg->spawn_count; ++i)
					add_particle(e, window.registry()->create());
		}

		void add_particle(entt::entity parent, entt::entity particle) noexcept {
			KENGINE_PROFILING_SCOPE;

			++particle_count;

			r.emplace<data::particle_timer>(particle);
			r.emplace<kengine::render::no_shadow>(particle);

			auto & instance_storage = r.storage<kengine::model::instance>();
			instance_storage.emplace(particle, instance_storage.get(parent));

			auto & drawable_storage = r.storage<kengine::render::drawable>();
			drawable_storage.emplace(particle, drawable_storage.get(parent));

			auto & particle_transform = r.emplace<kengine::core::transform>(particle);
			particle_transform.bounding_box = r.get<kengine::core::transform>(parent).bounding_box;
			particle_transform.bounding_box.position += putils::vec3f{ 0.f, 1.f, 0.f };
			particle_transform.bounding_box.size *= putils::vec3f{ .1f, .1f, .1f };

			particle_transform.pitch = putils::rand<float>(0.f, 2.f * putils::pi);
			particle_transform.yaw = putils::rand<float>(0.f, 2.f * putils::pi);
			particle_transform.roll = putils::rand<float>(0.f, 2.f * putils::pi);

			const putils::vec3f dir{ putils::rand<float>(-1.f, 1.f), .5f, putils::rand<float>(-1.f, 1.f) };
			particle_transform.bounding_box.position += dir;

			auto & particle_physics = r.emplace<kengine::physics::inertia>(particle);
			particle_physics.movement = dir;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system, data::particle_timer);
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	physics_test::add_system(r);
}
