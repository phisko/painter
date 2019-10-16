#include "UISystem.hpp"

#include "EntityManager.hpp"
#include "Export.hpp"

#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/UIComponent.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new UISystem(em);
}

UISystem::UISystem(kengine::EntityManager & em) : System(em), _em(em) {
	onLoad("");
}

void UISystem::onLoad(const char *) noexcept {
}

void UISystem::execute() {
	const kengine::CameraComponent3f * cam = nullptr;
	for (const auto & [e, comp] : _em.getEntities<kengine::CameraComponent3f>())
		cam = &comp;

	if (cam == nullptr)
		return;

	for (auto & [e, transform, ui] : _em.getEntities<kengine::TransformComponent3f, UIComponent>()) {
		const auto & pos = transform.boundingBox.position;
		const auto & camPos = cam->frustrum.position;

		transform.yaw = pos.getYawTo(camPos) + KENGINE_PI * 1.5f;
		transform.pitch = pos.getPitchTo(camPos);
	}
}
