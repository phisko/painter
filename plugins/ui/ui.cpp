// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/render/data/camera.hpp"
#include "kengine/render/data/sprite.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

// project
#include "data/ui.hpp"

namespace systems {
	struct ui {
		entt::registry & r;

		ui(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::main_loop::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			const kengine::render::camera * cam = nullptr;
			for (const auto & [e, comp] : r.view<kengine::render::camera>().each())
				cam = &comp;

			if (cam == nullptr)
				return;

			for (auto [e, transform] : r.view<kengine::core::transform, data::ui>().each()) {
				const auto & pos = transform.bounding_box.position;
				const auto & cam_pos = cam->frustum.position;

				const auto dir = putils::normalized(pos - cam_pos);
				transform.yaw = putils::get_yaw_from_normalized_direction(dir);
				transform.pitch = putils::get_pitch_from_normalized_direction(dir);
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(ui);
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	systems::add_ui(r);
}