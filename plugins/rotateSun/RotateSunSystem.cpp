#include "kengine.hpp"
#include "Export.hpp"

// putils
#include "angle.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/LightComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/pluginHelper.hpp"
#include "helpers/profilingHelper.hpp"

struct RotateSunSystem {
	static void init() noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine::entities += [](kengine::Entity & e) noexcept {
			e += kengine::functions::Execute{ execute };

			e += kengine::AdjustableComponent{
				"World", {
					{ "Sun rotation speed", &SUN_ROTATION }
				}
			};
		};
	}

	static void execute(float deltaTime) noexcept {
		KENGINE_PROFILING_SCOPE;

		for (auto [e, light] : kengine::entities.with<kengine::DirLightComponent>()) {
			static float angle = 0.f;
			angle += deltaTime * SUN_ROTATION;
			if (angle > putils::pi * 2.f)
				angle -= putils::pi * 2.f;
			light.direction = { std::cos(angle), -1.f, std::sin(angle) };
		}
	}

	static inline auto SUN_ROTATION = .1f;
};

EXPORT void loadKenginePlugin(void * state) noexcept {
	kengine::pluginHelper::initPlugin(state);
	RotateSunSystem::init();
}
