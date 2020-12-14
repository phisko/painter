#include "EntityManager.hpp"
#include "Export.hpp"

#include "data/TransformComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/SpriteComponent.hpp"
#include "data/UIComponent.hpp"

#include "functions/Execute.hpp"

#include "helpers/pluginHelper.hpp"

#include "angle.hpp"

static kengine::EntityManager * g_em;

#pragma region declarations
static void execute(float deltaTime);
#pragma endregion
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::pluginHelper::initPlugin(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };
	};
}

static void execute(float deltaTime) {
	const kengine::CameraComponent * cam = nullptr;
	for (const auto & [e, comp] : g_em->getEntities<kengine::CameraComponent>())
		cam = &comp;

	if (cam == nullptr)
		return;

	for (auto [e, transform, ui] : g_em->getEntities<kengine::TransformComponent, UIComponent>()) {
		const auto & pos = transform.boundingBox.position;
		const auto & camPos = cam->frustum.position;

		const auto dir = putils::normalized(pos - camPos);
		transform.yaw = putils::getYawFromNormalizedDirection(dir);
		transform.pitch = putils::getPitchFromNormalizedDirection(dir);
	}
}
