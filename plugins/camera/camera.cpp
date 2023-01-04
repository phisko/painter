#include "Export.hpp"

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

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/camera.hpp"
#include "kengine/data/viewport.hpp"
#include "kengine/data/input.hpp"
#include "kengine/data/transform.hpp"
#include "kengine/data/light.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"
#include "kengine/functions/on_mouse_captured.hpp"

// kengine helpers
#include "kengine/helpers/camera_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace systems {
	struct camera {
		entt::registry & r;

		struct {
			float mouse_sensitivity = .005f;
			float movement_speed = 10.f;
			float movement_speed_modifier = 2.f;
			float zoom_speed = .1f;
		} adjustables;

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

		camera(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			init_keys();

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::data::input>(camera_controller());
			e.emplace<kengine::data::adjustable>() = {
				"Camera",
				{
					{ "Mouse sensitivity", &adjustables.mouse_sensitivity },
					{ "Movement speed", &adjustables.movement_speed },
					{ "Movement speed modifier", &adjustables.movement_speed_modifier },
					{ "Zoom speed", &adjustables.zoom_speed },
				}
			};
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

			auto & comp = r.get<kengine::data::camera>(captured_camera);
			if (first) {
				update_vectors(comp);
				first = false;
			}

			auto & pos = comp.frustum.position;
			process_keys(pos, delta_time);
		}

		void process_keys(putils::point3f & pos, float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto velocity = adjustables.movement_speed * delta_time;

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
				adjustables.movement_speed /= adjustables.movement_speed_modifier;
			if (keys[int(inputs::speed_up)].pressed)
				adjustables.movement_speed *= adjustables.movement_speed_modifier;
		}

		void update_vectors(const kengine::data::camera & cam) noexcept {
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
		kengine::data::input camera_controller() noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine::data::input input;
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

			const float xoffset = movement.x * adjustables.mouse_sensitivity;
			const float yoffset = movement.y * adjustables.mouse_sensitivity;

			auto & cam = r.get<kengine::data::camera>(captured_camera);

			cam.yaw -= xoffset;
			cam.pitch -= yoffset;

			const auto pitchLimit = putils::pi / 2.f - .001f;
			cam.pitch = std::min(cam.pitch, pitchLimit);
			cam.pitch = std::max(cam.pitch, -pitchLimit);

			update_vectors(cam);
		}

		void process_mouse_scroll(float yoffset) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & cam = r.get<kengine::data::camera>(captured_camera);

			auto & zoom = cam.frustum.size.y;
			if (zoom >= .001f && zoom <= putils::pi * .9f)
				zoom -= yoffset * adjustables.zoom_speed;
			zoom = std::max(zoom, .001f);
			zoom = std::min(zoom, putils::pi * .9f);
		}

		void toggle_mouse_capture(entt::handle window, const putils::point2f & last_mouse_pos) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (captured_camera != entt::null)
				captured_camera = entt::null;
			else {
				const auto info = kengine::camera_helper::get_viewport_for_pixel(window, last_mouse_pos);
				captured_camera = info.camera;
			}

			for (const auto & [e, func] : r.view<kengine::functions::on_mouse_captured>().each())
				func(window, captured_camera != entt::null);
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::camera>(e);
}
