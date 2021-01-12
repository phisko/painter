#include "kengine.hpp"
#include "Export.hpp"
#include "helpers/pluginHelper.hpp"
#include "EntityCreator.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/PathfindingComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/NoShadowComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#include "rand.hpp"
#include "imgui.h"

#include "angle.hpp"

struct ParticleTimerComponent {
	float lifeTime = 0.f;
};

#pragma region Adjustables
static float PARTICLE_DURATION = 5.f;
static int SPAWN_COUNT = 1;
#pragma endregion Adjustables

static size_t g_particleCount = 0;

EXPORT void loadKenginePlugin(void * state) noexcept {
	struct impl {
		static void init() noexcept {
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
			for (const auto & [e, timer] : kengine::entities.with<ParticleTimerComponent>()) {
				timer.lifeTime += deltaTime;
				if (timer.lifeTime >= PARTICLE_DURATION) {
					kengine::entities.remove(e);
					--g_particleCount;
				}
			}

			if (ImGui::Begin("Particles"))
				ImGui::Text("Count: %zu", g_particleCount);
			ImGui::End();
		}

		static void onKey(kengine::EntityID window, int keycode, bool pressed) noexcept {
			if (!pressed || keycode != 'Y')
				return;
			for (auto [e, physics] : kengine::entities.with<kengine::PathfindingComponent>())
				for (int i = 0; i < SPAWN_COUNT; ++i)
					kengine::entities += Particle(e);
		}

		static kengine::EntityCreatorFunctor<64> Particle(kengine::Entity parent) noexcept {
			return [&](kengine::Entity & particle) noexcept {
				++g_particleCount;

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
	};

	kengine::pluginHelper::initPlugin(state);
	impl::init();
}
