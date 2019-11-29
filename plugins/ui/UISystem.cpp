#include "UISystem.hpp"

#include "EntityManager.hpp"
#include "Export.hpp"

#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/UIComponent.hpp"

#include "angle.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new UISystem(em);
}

UISystem::UISystem(kengine::EntityManager & em) : System(em), _em(em) {
}

void UISystem::execute() {
	const kengine::CameraComponent3f * cam = nullptr;
	for (const auto & [e, comp] : _em.getEntities<kengine::CameraComponent3f>())
		cam = &comp;

	if (cam == nullptr)
		return;

	for (auto & [e, transform, ui] : _em.getEntities<kengine::TransformComponent3f, UIComponent>()) {
		const auto & pos = transform.boundingBox.position;
		const auto & camPos = cam->frustum.position;

		const auto dir = putils::normalized(pos - camPos);
		transform.yaw = putils::getYawFromNormalizedDirection(dir);
		transform.pitch = putils::getPitchFromNormalizedDirection(dir);
	}
}
