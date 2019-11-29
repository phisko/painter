#include "CharacterMovementSystem.hpp"

#include "EntityManager.hpp"
#include "Export.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/PhysicsComponent.hpp"
#include "components/CharacterMovementComponent.hpp"

// DEBUG
#include "components/DebugGraphicsComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "imgui.h"

#include "sign.hpp"
#include "angle.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new CharacterMovementSystem(em);
}

static float FACING_STRICTNESS = 0.05f;
static float TURN_SPEED = putils::pi;
static float YAW_OFFSET = putils::pi / 2.f;

CharacterMovementSystem::CharacterMovementSystem(kengine::EntityManager & em) : System(em), _em(em) {
	em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Character/Movement] Facing strictness", &FACING_STRICTNESS); };
	em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Character/Movement] Turn speed", &TURN_SPEED); };
	em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Character/Movement] Yaw offset", &YAW_OFFSET); };
}

static float getRotationDirection(float angleDelta);
static void debug(kengine::EntityManager & em, kengine::Entity & e, const putils::Point3f & pos, const putils::Point3f & target);
void CharacterMovementSystem::execute() {
	for (auto & [e, transform, physics, movement] : _em.getEntities<kengine::TransformComponent3f, kengine::PhysicsComponent, CharacterMovementComponent>()) {
		e += kengine::KinematicComponent{};

		const auto & pos = transform.boundingBox.position;
		if (pos.getDistanceTo(movement.destination) > movement.targetDistance) {
			physics.movement = movement.destination - pos;
			physics.movement.normalize();
		}
		else {
			physics.movement = { 0.f, 0.f, 0.f };
		}

		const auto yawTo = pos.getYawTo(movement.destination);
		const auto yawDelta = transform.yaw - putils::constrainAngle(yawTo + YAW_OFFSET);
		if (std::abs(yawDelta) > FACING_STRICTNESS)
			physics.yaw = TURN_SPEED * getRotationDirection(yawDelta);
		else
			physics.yaw = 0.f;

		const auto pitchTo = pos.getPitchTo(movement.destination);
		const auto pitchDelta = transform.pitch - pitchTo;
		if (std::abs(pitchDelta) > FACING_STRICTNESS)
			physics.pitch = TURN_SPEED * getRotationDirection(pitchDelta);
		else
			physics.pitch = 0.f;

#ifndef KENGINE_NDEBUG
		debug(_em, e, pos, movement.destination);
#endif
	}
}

static float getRotationDirection(float angleDelta) {
	return std::abs(angleDelta) > putils::pi ?
		putils::sign(angleDelta) : -putils::sign(angleDelta);
}

#ifndef KENGINE_NDEBUG
static void debug(kengine::EntityManager & em, kengine::Entity & e, const putils::Point3f & pos, const putils::Point3f & target) {
		struct DebugEntityComponent {
			kengine::Entity::ID id = kengine::Entity::INVALID_ID;
		};

		// DEBUG
		if (!e.has<DebugEntityComponent>()) {
			auto & comp = e.attach<DebugEntityComponent>();
			em += [&](kengine::Entity & e) {
				comp.id = e.id;
				e += kengine::DebugGraphicsComponent(kengine::DebugGraphicsComponent::Line);
				e += kengine::TransformComponent3f{};
			};
			e += kengine::DebugGraphicsComponent(kengine::DebugGraphicsComponent::Sphere, { {}, { .1f, .1f, .1f } });
		}
		// /DEBUG

		auto & debug = em.getEntity(e.get<DebugEntityComponent>().id).attach<kengine::DebugGraphicsComponent>();
		debug.offset.position = pos;
		debug.lineEnd = target;
}
#endif
