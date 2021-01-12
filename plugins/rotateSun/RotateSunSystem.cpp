#include "kengine.hpp"
#include "Export.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/LightComponent.hpp"

#include "functions/Execute.hpp"

#include "angle.hpp"

#include "helpers/pluginHelper.hpp"

static auto SUN_ROTATION = .1f;

EXPORT void loadKenginePlugin(void * state) noexcept {
	struct impl {
		static void init() noexcept {
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
			for (auto [e, light] : kengine::entities.with<kengine::DirLightComponent>()) {
				static float angle = 0.f;
				angle += deltaTime * SUN_ROTATION;
				if (angle > putils::pi * 2.f)
					angle -= putils::pi * 2.f;
				light.direction = { std::cos(angle), -1.f, std::sin(angle) };
			}
		}
	};

	kengine::pluginHelper::initPlugin(state);
	impl::init();
}
