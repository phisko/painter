// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// imgui
#include <imgui.h>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"
#include "putils/vector.hpp"
#include "putils/plugin_manager/export.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/input/data/handler.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/render/data/camera.hpp"
#include "kengine/render/data/light.hpp"
#include "kengine/render/data/viewport.hpp"
#include "kengine/render/functions/on_mouse_captured.hpp"
#include "kengine/render/helpers/get_viewport_for_pixel.hpp"
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

#include "config.hpp"

namespace camera {
	struct system {
		entt::registry & r;
		const config * cfg = nullptr;

		enum class inputs {
			forward,
			backward,
			left,
			right,
			up,
			down,
			speed_up,
			speed_down,
		};

		struct key {
			int code;
			bool pressed;
		};

		key keys[magic_enum::enum_count<inputs>()];

		entt::entity captured_camera = entt::null;
		putils::vec3f front;
		putils::vec3f right;
		putils::vec3f up;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			init_keys();

			e.emplace<kengine::main_loop::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::input::handler>(camera_controller());

			e.emplace<kengine::core::name>("Camera");
			e.emplace<kengine::config::configurable>();
			cfg = &e.emplace<config>();
		}

		void init_keys() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto & key : keys)
				key.pressed = false;

			keys[int(inputs::forward)].code = 'W';
			keys[int(inputs::backward)].code = 'S';
			keys[int(inputs::left)].code = 'A';
			keys[int(inputs::right)].code = 'D';

			keys[int(inputs::up)].code = 'R';
			keys[int(inputs::down)].code = 'F';

			keys[int(inputs::speed_up)].code = GLFW_KEY_LEFT_SHIFT;
			keys[int(inputs::speed_down)].code = GLFW_KEY_LEFT_CONTROL;
		}

		bool first = true;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (captured_camera == entt::null)
				return;

			auto & comp = r.get<kengine::render::camera>(captured_camera);
			if (first) {
				update_vectors(comp);
				first = false;
			}

			auto & pos = comp.frustum.position;
			process_keys(pos, delta_time);
		}

		float movement_speed = 1.f;
		void process_keys(putils::point3f & pos, float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto velocity = movement_speed * cfg->movement_speed * delta_time;

			if (keys[int(inputs::forward)].pressed)
				pos += front * velocity;
			if (keys[int(inputs::backward)].pressed)
				pos -= front * velocity;
			if (keys[int(inputs::left)].pressed)
				pos -= right * velocity;
			if (keys[int(inputs::right)].pressed)
				pos += right * velocity;
			if (keys[int(inputs::up)].pressed)
				pos += up * velocity;
			if (keys[int(inputs::down)].pressed)
				pos -= up * velocity;

			if (keys[int(inputs::speed_down)].pressed)
				movement_speed /= cfg->movement_speed_modifier;
			if (keys[int(inputs::speed_up)].pressed)
				movement_speed *= cfg->movement_speed_modifier;
		}

		void update_vectors(const kengine::render::camera & cam) noexcept {
			KENGINE_PROFILING_SCOPE;

			front = {
				std::sin(cam.yaw) * std::cos(cam.pitch),
				std::sin(cam.pitch),
				std::cos(cam.yaw) * std::cos(cam.pitch)
			};
			putils::normalize(front);

			right = putils::cross(front, { 0.f, 1.f, 0.f });
			putils::normalize(right);

			up = putils::cross(right, front);
			putils::normalize(up);
		}

		putils::point2f last_mouse_pos;
		kengine::input::handler camera_controller() noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine::input::handler input;
			input.on_mouse_move = [this](entt::handle window, const putils::point2f & coords, const putils::point2f & rel) noexcept {
				last_mouse_pos = coords;
				if (captured_camera != entt::null)
					process_mouse_movement(rel);
			};
			input.on_scroll = [this](entt::handle window, float deltaX, float deltaY, const putils::point2f & coords) noexcept {
				if (captured_camera != entt::null)
					process_mouse_scroll(deltaY);
			};
			input.on_key = [this](entt::handle window, int key, bool pressed) noexcept {
				if (pressed && key == GLFW_KEY_ENTER)
					toggle_mouse_capture(window, last_mouse_pos);

				for (auto & k : keys)
					if (k.code == key)
						k.pressed = pressed;
			};

			return input;
		}

		void process_mouse_movement(const putils::point2f & movement) noexcept {
			KENGINE_PROFILING_SCOPE;

			const float xoffset = movement.x * cfg->mouse_sensitivity;
			const float yoffset = movement.y * cfg->mouse_sensitivity;

			auto & cam = r.get<kengine::render::camera>(captured_camera);

			cam.yaw -= xoffset;
			cam.pitch -= yoffset;

			const auto pitchLimit = putils::pi / 2.f - .001f;
			cam.pitch = std::min(cam.pitch, pitchLimit);
			cam.pitch = std::max(cam.pitch, -pitchLimit);

			update_vectors(cam);
		}

		void process_mouse_scroll(float yoffset) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & cam = r.get<kengine::render::camera>(captured_camera);

			auto & zoom = cam.frustum.size.y;
			if (zoom >= .001f && zoom <= putils::pi * .9f)
				zoom -= yoffset * cfg->zoom_speed;
			zoom = std::max(zoom, .001f);
			zoom = std::min(zoom, putils::pi * .9f);
		}

		void toggle_mouse_capture(entt::handle window, const putils::point2f & last_mouse_pos) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (captured_camera != entt::null)
				captured_camera = entt::null;
			else {
				const auto info = kengine::render::get_viewport_for_pixel(window, last_mouse_pos);
				captured_camera = info.camera;
			}

			for (const auto & [e, func] : r.view<kengine::render::on_mouse_captured>().each())
				func(window, captured_camera != entt::null);
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	camera::add_system(r);
}
