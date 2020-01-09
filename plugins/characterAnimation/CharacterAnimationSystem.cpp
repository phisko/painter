#include "EntityManager.hpp"
#include "helpers/PluginHelper.hpp"
#include "Export.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#include "registerTypes.hpp"
#include "sign.hpp"

struct WobbleComponent {
	float targetRoll = 0.f;
	putils_reflection_class_name(WobbleComponent);
};

static kengine::EntityManager * g_em;

static float WOBBLE_SPEED = 1.f;
static float MAX_ROLL = 1.f;

// declarations
static void execute(float deltaTime);
//
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::PluginHelper::initPlugin(em);

	registerComponents<WobbleComponent>(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };

		e += kengine::AdjustableComponent{
			"Character/Wobble", {
				{ "Speed", &WOBBLE_SPEED },
				{ "Max roll", &MAX_ROLL }
			}
		};
	};
}

static void execute(float deltaTime) {
	return;

	for (auto & [e, transform, physics, kinematic] : g_em->getEntities<kengine::TransformComponent, kengine::PhysicsComponent, kengine::KinematicComponent>()) {
		auto & wobble = e.attach<WobbleComponent>();

		const auto moving = physics.movement.getLengthSquared() > 0.f;
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