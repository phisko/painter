#include "kengine.hpp"
#include "Export.hpp"
#include "EntityCreator.hpp"

// imgui
#include <imgui.h>

// putils
#include "rand.hpp"
#include "angle.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/PathfindingComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/NoShadowComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/pluginHelper.hpp"
#include "helpers/profilingHelper.hpp"

struct PhysicsTestSystem {
	static void init() noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine::entities += [](kengine::Entity & e) noexcept {
			e += kengine::functions::Execute{ execute };

			e += kengine::AdjustableComponent{
				"Physics/Debug", {
					{ "Spawn count", &SPAWN_COUNT },
					{ "Duration", &PARTICLE_DURATION }
				}
			};

			e += kengine::InputComponent{ .onKey = onKey };
		};
	}

	static void execute(float deltaTime) noexcept {
		KENGINE_PROFILING_SCOPE;

		for (const auto & [e, timer] : kengine::entities.with<ParticleTimerComponent>()) {
			timer.lifeTime += deltaTime;
			if (timer.lifeTime >= PARTICLE_DURATION) {
				kengine::entities.remove(e);
				--_particleCount;
			}
		}

		if (ImGui::Begin("Particles"))
			ImGui::Text("Count: %zu", _particleCount);
		ImGui::End();
	}

	static void onKey(kengine::EntityID window, int keycode, bool pressed) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (!pressed || keycode != 'Y')
			return;
		for (auto [e, physics] : kengine::entities.with<kengine::PathfindingComponent>())
			for (int i = 0; i < SPAWN_COUNT; ++i)
				kengine::entities += Particle(e);
	}

	static kengine::EntityCreatorFunctor<64> Particle(kengine::Entity parent) noexcept {
		KENGINE_PROFILING_SCOPE;

		return [&](kengine::Entity & particle) noexcept {
			KENGINE_PROFILING_SCOPE;

			++_particleCount;

			particle += ParticleTimerComponent{};
			particle += parent.get<kengine::GraphicsComponent>();
			particle += parent.get<kengine::InstanceComponent>();
			particle += kengine::NoShadowComponent{};

			auto & particleTrans = particle.attach<kengine::TransformComponent>();
			particleTrans.boundingBox = parent.get<kengine::TransformComponent>().boundingBox;
			particleTrans.boundingBox.position += putils::Vector3f{ 0.f, 1.f, 0.f };
			particleTrans.boundingBox.size *= putils::Vector3f{ .1f, .1f, .1f };

			particleTrans.pitch = putils::rand<float>(0.f, 2.f * putils::pi);
			particleTrans.yaw = putils::rand<float>(0.f, 2.f * putils::pi);
			particleTrans.roll = putils::rand<float>(0.f, 2.f * putils::pi);

			const putils::Vector3f dir{ putils::rand<float>(-1.f, 1.f), .5f, putils::rand<float>(-1.f, 1.f) };
			particleTrans.boundingBox.position += dir;
			auto & particlePhys = particle.attach<kengine::PhysicsComponent>();
			particlePhys.movement = dir;
		};
	}

	struct ParticleTimerComponent {
		float lifeTime = 0.f;
	};

	static inline float PARTICLE_DURATION = 5.f;
	static inline int SPAWN_COUNT = 1;

	static inline size_t _particleCount = 0;
};

EXPORT void loadKenginePlugin(void * state) noexcept {
	kengine::pluginHelper::initPlugin(state);
	PhysicsTestSystem::init();
}
