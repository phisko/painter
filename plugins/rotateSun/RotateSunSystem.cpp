#include "RotateSunSystem.hpp"
#include "EntityManager.hpp"
#include "Export.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/LightComponent.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new RotateSunSystem(em);
}

static auto SUN_ROTATION = .1f;

RotateSunSystem::RotateSunSystem(kengine::EntityManager & em) : System(em), _em(em) {
	onLoad("");
}

#include "components/ImGuiComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/TextComponent.hpp"
#include "imgui.h"

void RotateSunSystem::onLoad(const char *) noexcept {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[World] Sun rotation speed", &SUN_ROTATION); };

	_em += [&](kengine::Entity & e) {
		e += kengine::ImGuiComponent([&] {
			if (ImGui::Begin("Billboards")) {
				const kengine::CameraComponent3f * cam = nullptr;
				for (const auto & [e, comp] : _em.getEntities<kengine::CameraComponent3f>())
					cam = &comp;

				if (cam != nullptr) {
					for (const auto & [e, transform, text] : _em.getEntities<kengine::TransformComponent3f, kengine::TextComponent3D>()) {
						const auto & pos = transform.boundingBox.position;
						ImGui::Text("XY: %f", pos.angleToXY(cam->frustrum.position));
						ImGui::Text("XZ: %f", pos.angleToXZ(cam->frustrum.position));
					}
				}
			}
			ImGui::End();
		});
	};
}

void RotateSunSystem::execute() {
	for (auto & [e, light] : _em.getEntities<kengine::DirLightComponent>()) {
		static float angle = 0.f;
		angle += time.getDeltaTime().count() * SUN_ROTATION;
		if (angle > PI * 2)
			angle -= PI * 2;
		light.direction = { std::cos(angle), -1.f, std::sin(angle) };
	}	

	const kengine::CameraComponent3f * cam = nullptr;
	for (const auto & [e, comp] : _em.getEntities<kengine::CameraComponent3f>())
		cam = &comp;

	if (cam == nullptr)
		return;

	for (auto & [e, transform, text] : _em.getEntities<kengine::TransformComponent3f, kengine::TextComponent3D>()) {
		const auto & pos = transform.boundingBox.position;
		transform.pitch = std::fmod(pos.angleToXY(cam->frustrum.position) + PI / 2, PI);
		transform.yaw = pos.angleToXZ(cam->frustrum.position) - PI / 2;
	}
}
