#include "kengine.hpp"
#include "Export.hpp"

// putils
#include "sign.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/pluginHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/registerTypeHelper.hpp"

struct WobbleComponent {
	float targetRoll = 0.f;
};

#define refltype WobbleComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

struct CharacterAnimationSystem {
	static void init() noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine::registerComponents<WobbleComponent>();

		kengine::entities += [](kengine::Entity & e) noexcept {
			e += kengine::functions::Execute{ execute };

			e += kengine::AdjustableComponent{
				"Character/Wobble", {
					{ "Speed", &WOBBLE_SPEED },
					{ "Max roll", &MAX_ROLL }
				}
			};
		};
	}

	static void execute(float deltaTime) noexcept {
		KENGINE_PROFILING_SCOPE;

		return;

		for (auto [e, transform, physics, kinematic] : kengine::entities.with<kengine::TransformComponent, kengine::PhysicsComponent, kengine::KinematicComponent>()) {
			auto & wobble = e.attach<WobbleComponent>();

			const auto moving = putils::getLengthSquared(physics.movement) > 0.f;
			if (!moving)
				wobble.targetRoll = 0.f;
			else if (wobble.targetRoll == 0.f)
				wobble.targetRoll = MAX_ROLL;

			const auto deltaRoll = wobble.targetRoll - transform.roll;
			physics.roll = WOBBLE_SPEED * putils::sign(deltaRoll);
			if (std::abs(deltaRoll) < .05f) {
				if (moving)
					wobble.targetRoll = wobble.targetRoll <= 0.f ? MAX_ROLL : -MAX_ROLL;
				else
					physics.roll = 0.f;
			}
		}
	}

	static inline float WOBBLE_SPEED = 1.f;
	static inline float MAX_ROLL = 1.f;
};

EXPORT void loadKenginePlugin(void * state) noexcept {
	kengine::pluginHelper::initPlugin(state);
	CharacterAnimationSystem::init();
}