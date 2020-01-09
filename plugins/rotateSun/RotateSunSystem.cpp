#include "EntityManager.hpp"
#include "Export.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/LightComponent.hpp"

#include "functions/Execute.hpp"

#include "angle.hpp"

#include "helpers/PluginHelper.hpp"

static kengine::EntityManager * g_em;

static auto SUN_ROTATION = .1f;

static void execute(float deltaTime);
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::PluginHelper::initPlugin(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };

		e += kengine::AdjustableComponent{
			"World", {
				{ "Sun rotation speed", &SUN_ROTATION }
			}
		};
	};
}

static void execute(float deltaTime) {
	for (auto & [e, light] : g_em->getEntities<kengine::DirLightComponent>()) {
		static float angle = 0.f;
		angle += deltaTime * SUN_ROTATION;
		if (angle > putils::pi * 2.f)
			angle -= putils::pi * 2.f;
		light.direction = { std::cos(angle), -1.f, std::sin(angle) };
	}	
}
