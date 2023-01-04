#include "Export.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/rand.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/pathfinding.hpp"
#include "kengine/data/graphics.hpp"
#include "kengine/data/instance.hpp"
#include "kengine/data/input.hpp"
#include "kengine/data/no_shadow.hpp"
#include "kengine/data/physics.hpp"
#include "kengine/data/transform.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace data {
		struct particle_timer {
			float lifetime = 0.f;
		};
}

namespace systems {
	struct physics_test {
		entt::registry & r;

		struct {
			float particle_duration = 5.f;
			int spawn_count = 1;
		} adjustables;

		size_t particle_count = 0;

		physics_test(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::data::adjustable>() = {
				"Physics/Debug",
				{
					{ "Spawn count", &adjustables.spawn_count },
					{ "Duration", &adjustables.particle_duration },
				}
			};
			e.emplace<kengine::data::input>() = { .on_key = putils_forward_to_this(on_key) };
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [e, timer] : r.view<data::particle_timer>().each()) {
				timer.lifetime += delta_time;
				if (timer.lifetime >= adjustables.particle_duration) {
					r.destroy(e);
					--particle_count;
				}
			}

			if (ImGui::Begin("Particles"))
				ImGui::Text("Count: %zu", particle_count);
			ImGui::End();
		}

		void on_key(entt::handle window, int keycode, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed || keycode != 'Y')
				return;
			for (auto [e, physics] : window.registry()->view<kengine::data::pathfinding>().each())
				for (int i = 0; i < adjustables.spawn_count; ++i) {
					add_particle(e, window.registry()->create());
				}
		}

		void add_particle(entt::entity parent, entt::entity particle) noexcept {
			KENGINE_PROFILING_SCOPE;

			++particle_count;

			r.emplace<data::particle_timer>(particle);
			r.emplace<kengine::data::no_shadow>(particle);

			auto & graphics_storage = r.storage<kengine::data::graphics>();
			graphics_storage.emplace(particle, graphics_storage.get(parent));

			auto & instance_storage = r.storage<kengine::data::instance>();
			instance_storage.emplace(particle, instance_storage.get(parent));

			auto & particle_transform = r.emplace<kengine::data::transform>(particle);
			particle_transform.bounding_box = r.get<kengine::data::transform>(parent).bounding_box;
			particle_transform.bounding_box.position += putils::vec3f{ 0.f, 1.f, 0.f };
			particle_transform.bounding_box.size *= putils::vec3f{ .1f, .1f, .1f };

			particle_transform.pitch = putils::rand<float>(0.f, 2.f * putils::pi);
			particle_transform.yaw = putils::rand<float>(0.f, 2.f * putils::pi);
			particle_transform.roll = putils::rand<float>(0.f, 2.f * putils::pi);

			const putils::vec3f dir{ putils::rand<float>(-1.f, 1.f), .5f, putils::rand<float>(-1.f, 1.f) };
			particle_transform.bounding_box.position += dir;

			auto & particle_physics = r.emplace<kengine::data::physics>(particle);
			particle_physics.movement = dir;
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::physics_test>(e);
}
