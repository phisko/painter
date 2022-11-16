#include "kengine.hpp"
#include "Export.hpp"

// glfw
#include <GLFW/glfw3.h>

// putils
#include "sign.hpp"
#include "angle.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/PathfindingComponent.hpp"
#include "data/NavMeshComponent.hpp"

#ifndef KENGINE_NDEBUG
#include "data/DebugGraphicsComponent.hpp"
#endif

// kengine functions
#include "functions/Execute.hpp"
#include "functions/GetPositionInPixel.hpp"

// kengine helpers
#include "helpers/pluginHelper.hpp"
#include "helpers/instanceHelper.hpp"
#include "helpers/profilingHelper.hpp"

struct CharacterMovementSystem {
	static void init() noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine::entities += [](kengine::Entity & e) noexcept {
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

	static void click(kengine::EntityID window, int button, const putils::Point3f & screenCoordinates, bool pressed) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (!pressed || button != GLFW_MOUSE_BUTTON_RIGHT)
			return;

		for (const auto & [e, getPositionInPixel] : kengine::entities.with<kengine::functions::GetPositionInPixel>()) {
			const auto pos = getPositionInPixel(window, screenCoordinates);
			if (!pos)
				continue;
			for (const auto & [e, pathfinding] : kengine::entities.with<kengine::PathfindingComponent>())
				pathfinding.destination = *pos;
		}
	}

	static void execute(float deltaTime) noexcept {
		KENGINE_PROFILING_SCOPE;

		initPositionGBuffer();
		setEnvironments();
		updateOrientations();
	}

	static void initPositionGBuffer() noexcept {
		KENGINE_PROFILING_SCOPE;

		static bool first = true;
		if (first) {
			for (const auto & [e, getPositionInPixel] : kengine::entities.with<kengine::functions::GetPositionInPixel>())
				getPositionInPixel(kengine::INVALID_ID, { 0, 0 });
			first = false;
		}
	}

	static void setEnvironments() noexcept {
		KENGINE_PROFILING_SCOPE;

		for (const auto & [e, pathfinding] : kengine::entities.with<kengine::PathfindingComponent>()) {
			if (pathfinding.environment != kengine::INVALID_ID)
				continue;
			for (const auto & [e, instance] : kengine::entities.with<kengine::InstanceComponent>())
				if (kengine::instanceHelper::modelHas<kengine::NavMeshComponent>(instance)) {
					pathfinding.environment = e.id;
					break;
				}
		}
	}

	static void updateOrientations() noexcept {
		KENGINE_PROFILING_SCOPE;

		for (auto [e, transform, physics, pathfinding] : kengine::entities.with<kengine::TransformComponent, kengine::PhysicsComponent, kengine::PathfindingComponent>()) {
			if (pathfinding.environment == kengine::INVALID_ID)
				continue;

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

	static void debug(kengine::Entity & e, const putils::Point3f & pos, const kengine::PathfindingComponent & pathfinding) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto obj = kengine::entities[pathfinding.environment];
		const auto & getPath = kengine::instanceHelper::getModel<kengine::functions::GetPath>(obj);
		const auto path = getPath(obj, pos, pathfinding.destination);

		auto & debug = e.attach<kengine::DebugGraphicsComponent>();
		debug.elements.clear();

		bool first = true;
		putils::Point3f lastPos;
		for (auto pos : path) {
			pos.y += 1.f;

			kengine::DebugGraphicsComponent::Element debugElement;  {
				debugElement.type = kengine::DebugGraphicsComponent::Type::Sphere;
				debugElement.sphere.radius = .1f;
				debugElement.pos = pos;
				debugElement.referenceSpace = kengine::DebugGraphicsComponent::ReferenceSpace::World;
			};
			debug.elements.emplace_back(std::move(debugElement));

			if (!first) {
				kengine::DebugGraphicsComponent::Element debugElement; {
					debugElement.type = kengine::DebugGraphicsComponent::Type::Line;
					debugElement.line.end = lastPos;
					debugElement.pos = pos;
					debugElement.referenceSpace = kengine::DebugGraphicsComponent::ReferenceSpace::World;
				}
				debug.elements.emplace_back(std::move(debugElement));
			}

			first = false;
			lastPos = pos;
		}
	}

	static inline float FACING_STRICTNESS = 0.05f;
	static inline float TURN_SPEED = putils::pi;
};

EXPORT void loadKenginePlugin(void * state) noexcept {
	kengine::pluginHelper::initPlugin(state);
	CharacterMovementSystem::init();
}
