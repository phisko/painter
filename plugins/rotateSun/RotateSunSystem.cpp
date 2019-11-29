#include "RotateSunSystem.hpp"
#include "EntityManager.hpp"
#include "Export.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/LightComponent.hpp"

#include "angle.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new RotateSunSystem(em);
}

static auto SUN_ROTATION = .1f;

RotateSunSystem::RotateSunSystem(kengine::EntityManager & em) : System(em), _em(em) {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[World] Sun rotation speed", &SUN_ROTATION); };
}

void RotateSunSystem::execute() {
	for (auto & [e, light] : _em.getEntities<kengine::DirLightComponent>()) {
		static float angle = 0.f;
		angle += time.getDeltaTime().count() * SUN_ROTATION;
		if (angle > putils::pi * 2.f)
			angle -= putils::pi * 2.f;
		light.direction = { std::cos(angle), -1.f, std::sin(angle) };
	}	
}
