#include <GLFW/glfw3.h>

#include "EntityManager.hpp"
#include "Export.hpp"
#include "helpers/pluginHelper.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/PathfindingComponent.hpp"
#include "data/NavMeshComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/GetPositionInPixel.hpp"

#include "helpers/instanceHelper.hpp"

#ifndef KENGINE_NDEBUG
#include "data/DebugGraphicsComponent.hpp"
#endif

#include "sign.hpp"
#include "angle.hpp"

static kengine::EntityManager * g_em;

static float FACING_STRICTNESS = 0.05f;
static float TURN_SPEED = putils::pi;

#pragma region declarations
static void click(kengine::Entity::ID window, int button, const putils::Point3f & screenCoordinates, bool pressed);
static void execute(float deltaTime);
#pragma endregion
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

		e += kengine::InputComponent{
			.onMouseButton = click 
		};
	};
}

static void click(kengine::Entity::ID window, int button, const putils::Point3f & screenCoordinates, bool pressed) {
	if (!pressed || button != GLFW_MOUSE_BUTTON_RIGHT)
		return;

	for (const auto & [e, getPositionInPixel] : g_em->getEntities<kengine::functions::GetPositionInPixel>()) {
		const auto pos = getPositionInPixel(window, screenCoordinates);
		for (const auto & [e, pathfinding] : g_em->getEntities<kengine::PathfindingComponent>())
			pathfinding.destination = pos;
	}
}

#pragma region execute
#pragma region declarations
static void initPositionGBuffer();
static void setEnvironments();
static void updateOrientations();
#pragma endregion
static void execute(float deltaTime) {
	initPositionGBuffer();
	setEnvironments();
	updateOrientations();
}

static void initPositionGBuffer() {
	static bool first = true;
	if (first) {
		for (const auto & [e, getPositionInPixel] : g_em->getEntities<kengine::functions::GetPositionInPixel>())
			getPositionInPixel(kengine::Entity::INVALID_ID, { 0, 0 });
		first = false;
	}
}

static void setEnvironments() {
	for (const auto & [e, pathfinding] : g_em->getEntities<kengine::PathfindingComponent>()) {
		if (pathfinding.environment != kengine::Entity::INVALID_ID)
			continue;
		for (const auto & [e, instance] : g_em->getEntities<kengine::InstanceComponent>())
			if (kengine::instanceHelper::modelHas<kengine::NavMeshComponent>(*g_em, instance)) {
				pathfinding.environment = e.id;
				break;
			}
	}
}

#pragma region updateOrientations
#pragma region declarations
static void debug(kengine::Entity & e, const putils::Point3f & pos, const kengine::PathfindingComponent & pathfinding);
#pragma endregion
static void updateOrientations() {
	for (auto [e, transform, physics, pathfinding] : g_em->getEntities<kengine::TransformComponent, kengine::PhysicsComponent, kengine::PathfindingComponent>()) {
		e += kengine::KinematicComponent{};

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

		debug(e, transform.boundingBox.position, pathfinding);
	}
}

static void debug(kengine::Entity & e, const putils::Point3f & pos, const kengine::PathfindingComponent & pathfinding) {
	const auto obj = g_em->getEntity(pathfinding.environment);
	const auto & navMesh = kengine::instanceHelper::getModel<kengine::NavMeshComponent>(*g_em, obj);

	const auto path = navMesh.getPath(obj, pos, pathfinding.destination);

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
#pragma endregion updateOrientations

#pragma endregion execute
