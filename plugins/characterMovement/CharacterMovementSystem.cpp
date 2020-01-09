#include "EntityManager.hpp"
#include "Export.hpp"
#include "helpers/PluginHelper.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/CharacterMovementComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#ifndef KENGINE_NDEBUG
#include "data/DebugGraphicsComponent.hpp"
#include "data/ImGuiComponent.hpp"
#include "imgui.h"
#endif

#include "sign.hpp"
#include "angle.hpp"

static kengine::EntityManager * g_em;

static float FACING_STRICTNESS = 0.05f;
static float TURN_SPEED = putils::pi;

// declarations
static void execute(float deltaTime);
//
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::PluginHelper::initPlugin(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };

		e += kengine::AdjustableComponent{
			"Character/Movement", {
				{ "Facing strictness", &FACING_STRICTNESS },
				{ "Turn speed", &TURN_SPEED }
			}
		};
	};
}

// declarations
static void debug(kengine::Entity & e, const putils::Point3f & pos, const putils::Point3f & target);
//
static void execute(float deltaTime) {
	for (auto & [e, transform, physics, movement] : g_em->getEntities<kengine::TransformComponent, kengine::PhysicsComponent, CharacterMovementComponent>()) {
		e += kengine::KinematicComponent{};

		const auto & pos = transform.boundingBox.position;
		const auto posToDest = movement.destination - pos;

#ifndef KENGINE_NDEBUG
		debug(e, pos, movement.destination);
#endif

		if (posToDest.getLength() <= movement.targetDistance) {
			physics.movement = { 0.f, 0.f, 0.f };
			physics.yaw = 0.f;
			physics.pitch = 0.f;
			continue;
		}

		physics.movement = movement.destination - pos;
		physics.movement.normalize();

		const auto direction = putils::normalized(movement.destination - pos);

		const auto yawTo = putils::getYawFromNormalizedDirection(direction);
		const auto yawDelta = putils::constrainAngle(putils::constrainAngle(yawTo) - transform.yaw);
		if (std::abs(yawDelta) > FACING_STRICTNESS)
			physics.yaw = TURN_SPEED * putils::sign(yawDelta);
		else
			physics.yaw = 0.f;

		const auto pitchTo = putils::getPitchFromNormalizedDirection(direction);
		const auto pitchDelta = putils::constrainAngle(putils::constrainAngle(pitchTo) - transform.pitch);
		if (std::abs(pitchDelta) > FACING_STRICTNESS)
			physics.pitch = TURN_SPEED * putils::sign(pitchDelta);
		else
			physics.pitch = 0.f;
	}
}

#ifndef KENGINE_NDEBUG
static void debug(kengine::Entity & e, const putils::Point3f & pos, const putils::Point3f & target) {
	struct DebugEntityComponent {
		kengine::Entity::ID id = kengine::Entity::INVALID_ID;
	};

	if (!e.has<DebugEntityComponent>()) {
		auto & comp = e.attach<DebugEntityComponent>();
		*g_em += [&](kengine::Entity & e) {
			comp.id = e.id;
			e += kengine::DebugGraphicsComponent(kengine::DebugGraphicsComponent::Line);
			e += kengine::TransformComponent{};
		};
		e += kengine::DebugGraphicsComponent(kengine::DebugGraphicsComponent::Sphere, { {}, { .1f, .1f, .1f } });
	}

	auto & debug = g_em->getEntity(e.get<DebugEntityComponent>().id).attach<kengine::DebugGraphicsComponent>();
	debug.offset.position = pos;
	debug.lineEnd = target;
}
#endif
