#include "EntityManager.hpp"
#include "Export.hpp"
#include "helpers/PluginHelper.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/CharacterMovementComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/ImGuiComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/NoShadowComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#include "rand.hpp"
#include "imgui.h"

#include "angle.hpp"

static kengine::EntityManager * g_em;

struct ParticleTimerComponent {
	float lifeTime = 0.f;
};

static float PARTICLE_DURATION = 5.f;
static int SPAWN_COUNT = 1;
static size_t g_particleCount = 0;

// declarations
static void execute(float deltaTime);
static void onKey(kengine::Entity::ID window, int keycode, bool pressed);
//
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::PluginHelper::initPlugin(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };

		e += kengine::AdjustableComponent{
			"Physics/Debug", {
				{ "Spawn count", &SPAWN_COUNT },
				{ "Duration", &PARTICLE_DURATION }
			}
		};

		e += kengine::ImGuiComponent([] {
			if (ImGui::Begin("Particles"))
				ImGui::Text("Count: %zu", g_particleCount);
			ImGui::End();
		});

		auto & input = e.attach<kengine::InputComponent>();
		input.onKey = onKey;
	};
}

static void execute(float deltaTime) {
	for (const auto & [e, timer] : g_em->getEntities<ParticleTimerComponent>()) {
		timer.lifeTime += deltaTime;
		if (timer.lifeTime >= PARTICLE_DURATION) {
			g_em->removeEntity(e.id);
			--g_particleCount;
		}
	}
}

// declarations
static kengine::EntityCreatorFunctor<64> Particle(kengine::Entity parent);
//
static void onKey(kengine::Entity::ID window, int keycode, bool pressed) {
	if (!pressed || keycode != 'Y')
		return;
	for (auto &[e, physics] : g_em->getEntities<CharacterMovementComponent>())
		for (int i = 0; i < SPAWN_COUNT; ++i)
			*g_em += Particle(e);
}

static kengine::EntityCreatorFunctor<64> Particle(kengine::Entity parent) {
	return [&](kengine::Entity & particle) {
		++g_particleCount;

		particle += ParticleTimerComponent{};
		particle += parent.get<kengine::GraphicsComponent>();
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