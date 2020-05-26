#include "EntityManager.hpp"
#include "Export.hpp"
#include "helpers/pluginHelper.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/CharacterMovementComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#include "helpers/instanceHelper.hpp"

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
	kengine::pluginHelper::initPlugin(em);

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
static void addPathComponent(kengine::Entity & e);
static void debug(kengine::Entity & e, const kengine::NavMeshComponent::Path & path, const putils::Point3f & target);
//
static void execute(float deltaTime) {
	for (auto & [e, transform, physics, movement] : g_em->getEntities<kengine::TransformComponent, kengine::PhysicsComponent, CharacterMovementComponent>()) {
		e += kengine::KinematicComponent{};

		if (!e.has<PathComponent>())
			addPathComponent(e);
		auto & path = e.get<PathComponent>();

		const auto obj = g_em->getEntity(path.navMesh);
		const auto & navMesh = kengine::instanceHelper::getModel<kengine::NavMeshComponent>(*g_em, obj);

		const auto & pos = transform.boundingBox.position;
		path.path = navMesh.getPath(obj, pos, movement.destination);
		if (path.path.empty()) // First step is current position
			continue;

#ifndef KENGINE_NDEBUG
		debug(e, path.path, movement.destination);
#endif

		const auto nextStep = [&] {
			for (size_t i = 0; i < path.path.size(); ++i) {
				const auto posToStep = path.path[i] - pos;
				if (posToStep.getLength() <= movement.targetDistance)
					continue;
				return i;
			}
			return (size_t)0;
		}();

		const auto posToDest = path.path[nextStep] - pos;

		if (posToDest.getLength() <= movement.targetDistance) {
			physics.movement = { 0.f, 0.f, 0.f };
			physics.yaw = 0.f;
			physics.pitch = 0.f;
			continue;
		}

		physics.movement = posToDest;
		physics.movement.normalize();

		const auto yawTo = putils::getYawFromNormalizedDirection(physics.movement);
		const auto yawDelta = putils::constrainAngle(putils::constrainAngle(yawTo) - transform.yaw);
		if (std::abs(yawDelta) > FACING_STRICTNESS)
			physics.yaw = TURN_SPEED * putils::sign(yawDelta);
		else
			physics.yaw = 0.f;

		const auto pitchTo = putils::getPitchFromNormalizedDirection(physics.movement);
		const auto pitchDelta = putils::constrainAngle(putils::constrainAngle(pitchTo) - transform.pitch);
		if (std::abs(pitchDelta) > FACING_STRICTNESS)
			physics.pitch = TURN_SPEED * putils::sign(pitchDelta);
		else
			physics.pitch = 0.f;
	}
}

static void addPathComponent(kengine::Entity & e) {
	auto & comp = e.attach<PathComponent>();
	for (const auto & [obj, instance] : g_em->getEntities<kengine::InstanceComponent>()) {
		if (!kengine::instanceHelper::modelHas<kengine::NavMeshComponent>(*g_em, instance))
			continue;
		comp.navMesh = obj.id;
	}
}

#ifndef KENGINE_NDEBUG
static void debug(kengine::Entity & e, const kengine::NavMeshComponent::Path & path, const putils::Point3f & target) {
	auto & debug = e.attach<kengine::DebugGraphicsComponent>();
	debug.elements.clear();

	bool first = true;
	putils::Point3f lastPos;
	for (auto pos : path) {
		pos.y += 1.f;

		auto & element = debug.elements.emplace_back(kengine::DebugGraphicsComponent::Sphere{ .1f });
		element.pos = pos;
		element.referenceSpace = kengine::DebugGraphicsComponent::ReferenceSpace::World;

		if (!first) {
			auto & element = debug.elements.emplace_back(kengine::DebugGraphicsComponent::Line{ lastPos });
			element.pos = pos;
			element.referenceSpace = kengine::DebugGraphicsComponent::ReferenceSpace::World;
		}

		first = false;
		lastPos = pos;
	}
}
#endif
