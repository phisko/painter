#include "Export.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/transform.hpp"
#include "kengine/data/camera.hpp"
#include "kengine/data/sprite.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

// project
#include "data/ui.hpp"

namespace systems {
	struct ui {
		entt::registry & r;

		ui(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			const kengine::data::camera * cam = nullptr;
			for (const auto & [e, comp] : r.view<kengine::data::camera>().each())
				cam = &comp;

			if (cam == nullptr)
				return;

			for (auto [e, transform] : r.view<kengine::data::transform, data::ui>().each()) {
				const auto & pos = transform.bounding_box.position;
				const auto & cam_pos = cam->frustum.position;

				const auto dir = putils::normalized(pos - cam_pos);
				transform.yaw = putils::get_yaw_from_normalized_direction(dir);
				transform.pitch = putils::get_pitch_from_normalized_direction(dir);
			}
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::ui>(e);
}