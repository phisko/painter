#include "kengine.hpp"
#include "Export.hpp"

// putils
#include "angle.hpp"

// kengine data
#include "data/TransformComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/SpriteComponent.hpp"
#include "data/UIComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/pluginHelper.hpp"
#include "helpers/profilingHelper.hpp"

struct UISystem {
	static void init() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine::entities += [](kengine::Entity & e) noexcept {
			e += kengine::functions::Execute{ execute };
		};
	}

	static void execute(float deltaTime) noexcept {
		KENGINE_PROFILING_SCOPE;

		const kengine::CameraComponent * cam = nullptr;
		for (const auto & [e, comp] : kengine::entities.with<kengine::CameraComponent>())
			cam = &comp;

		if (cam == nullptr)
			return;

		for (auto [e, transform, ui] : kengine::entities.with<kengine::TransformComponent, UIComponent>()) {
			const auto & pos = transform.boundingBox.position;
			const auto & camPos = cam->frustum.position;

			const auto dir = putils::normalized(pos - camPos);
			transform.yaw = putils::getYawFromNormalizedDirection(dir);
			transform.pitch = putils::getPitchFromNormalizedDirection(dir);
		}
	}
};

EXPORT void loadKenginePlugin(void * state) noexcept {
	kengine::pluginHelper::initPlugin(state);
	UISystem::init();
}
