#include "kengine.hpp"
#include "Export.hpp"

#include "data/TransformComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/SpriteComponent.hpp"
#include "data/UIComponent.hpp"

#include "functions/Execute.hpp"

#include "helpers/pluginHelper.hpp"

#include "angle.hpp"

EXPORT void loadKenginePlugin(void * state) noexcept {
	struct impl {
		static void init() noexcept {
			kengine::entities += [](kengine::Entity & e) noexcept {
				e += kengine::functions::Execute{ execute };
			};
		}

		static void execute(float deltaTime) noexcept {
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

	kengine::pluginHelper::initPlugin(state);
	impl::init();
}
