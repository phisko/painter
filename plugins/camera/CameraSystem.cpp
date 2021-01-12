#include "Export.hpp"
#include "kengine.hpp"
#include "vector.hpp"
#include "EntityCreator.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/InputComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/LightComponent.hpp"

#include "functions/Execute.hpp"

#include "functions/OnMouseCaptured.hpp"
#include "helpers/cameraHelper.hpp"
#include "helpers/pluginHelper.hpp"

#include "imgui.h"
#include "GLFW/glfw3.h"
#include "magic_enum.hpp"
#include "angle.hpp"

#pragma region Adjustables
static auto MOUSE_SENSITIVITY = .005f;
static auto MOVEMENT_SPEED = 10.f;
static auto MOVEMENT_SPEED_MODIFIER = 2.f;
static auto ZOOM_SPEED = .1f;
#pragma endregion Adjustables

enum Inputs {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	SPEED_UP,
	SPEED_DOWN,
};
struct Key {
	int code;
	bool pressed;
};
static Key keys[putils::magic_enum::enum_count<Inputs>()];

static kengine::EntityID g_capturedCamera = kengine::INVALID_ID;
static putils::Vector3f front;
static putils::Vector3f right;
static putils::Vector3f up;

EXPORT void loadKenginePlugin(void * state) noexcept {
	struct impl {
		static void init() noexcept {
			initKeys();

			kengine::entities += [](kengine::Entity & e) noexcept {
				e += kengine::functions::Execute{ execute };
				e += CameraController();

				e += kengine::AdjustableComponent{
					"Camera", {
						{ "Mouse sensitivity", &MOUSE_SENSITIVITY },
						{ "Movement speed", &MOVEMENT_SPEED },
						{ "Movement speed modifier", &MOVEMENT_SPEED_MODIFIER },
						{ "Zoom speed", &ZOOM_SPEED }
					}
				};
			};
		}

		static void initKeys() noexcept {
			for (auto & key : keys)
				key.pressed = false;

			keys[FORWARD].code = 'W';
			keys[BACKWARD].code = 'S';
			keys[LEFT].code = 'A';
			keys[RIGHT].code = 'D';

			keys[UP].code = 'R';
			keys[DOWN].code = 'F';

			keys[SPEED_UP].code = GLFW_KEY_LEFT_SHIFT;
			keys[SPEED_DOWN].code = GLFW_KEY_LEFT_CONTROL;
		}

		static void execute(float deltaTime) noexcept {
			if (g_capturedCamera == kengine::INVALID_ID)
				return;

			auto e = kengine::entities[g_capturedCamera];
			auto & comp = e.get<kengine::CameraComponent>();
			static bool first = true;
			if (first) {
				updateVectors(comp);
				first = false;
			}

			auto & pos = comp.frustum.position;
			processKeys(pos, deltaTime);
		}

		static void processKeys(putils::Point3f & pos, float deltaTime) noexcept {
			const auto velocity = MOVEMENT_SPEED * deltaTime;

			if (keys[FORWARD].pressed)
				pos += front * velocity;
			if (keys[BACKWARD].pressed)
				pos -= front * velocity;
			if (keys[LEFT].pressed)
				pos -= right * velocity;
			if (keys[RIGHT].pressed)
				pos += right * velocity;
			if (keys[UP].pressed)
				pos += up * velocity;
			if (keys[DOWN].pressed)
				pos -= up * velocity;

			if (keys[SPEED_DOWN].pressed)
				MOVEMENT_SPEED /= MOVEMENT_SPEED_MODIFIER;
			if (keys[SPEED_UP].pressed)
				MOVEMENT_SPEED *= MOVEMENT_SPEED_MODIFIER;
		}

		static void updateVectors(const kengine::CameraComponent & cam) noexcept {
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

		static kengine::InputComponent CameraController() noexcept {
			static putils::Point2f lastMousePos;

			kengine::InputComponent input;
			input.onMouseMove = [](kengine::EntityID window, const putils::Point2f & coords, const putils::Point2f & rel) noexcept {
				lastMousePos = coords;
				if (g_capturedCamera != kengine::INVALID_ID)
					processMouseMovement(rel);
			};
			input.onScroll = [](kengine::EntityID window, float deltaX, float deltaY, const putils::Point2f & coords) noexcept {
				if (g_capturedCamera != kengine::INVALID_ID)
					processMouseScroll(deltaY);
			};
			input.onKey = [](kengine::EntityID window, int key, bool pressed) noexcept {
				if (pressed && key == GLFW_KEY_ENTER)
					toggleMouseCapture(window, lastMousePos);

				for (auto & k : keys)
					if (k.code == key)
						k.pressed = pressed;
			};

			return input;
		}

		static void processMouseMovement(const putils::Point2f & movement) noexcept {
			const float xoffset = movement.x * MOUSE_SENSITIVITY;
			const float yoffset = movement.y * MOUSE_SENSITIVITY;

			auto e = kengine::entities[g_capturedCamera];
			auto & cam = e.get<kengine::CameraComponent>();

			cam.yaw -= xoffset;
			cam.pitch -= yoffset;

			const auto pitchLimit = putils::pi / 2.f - .001f;
			cam.pitch = std::min(cam.pitch, pitchLimit);
			cam.pitch = std::max(cam.pitch, -pitchLimit);

			updateVectors(cam);
		}

		static void processMouseScroll(float yoffset) noexcept {
			auto e = kengine::entities[g_capturedCamera];
			auto & cam = e.get<kengine::CameraComponent>();

			auto & zoom = cam.frustum.size.y;
			if (zoom >= .001f && zoom <= putils::pi * .9f)
				zoom -= yoffset * ZOOM_SPEED;
			zoom = std::max(zoom, .001f);
			zoom = std::min(zoom, putils::pi * .9f);
		}

		static void toggleMouseCapture(kengine::EntityID window, const putils::Point2f & lastMousePos) noexcept {
			if (g_capturedCamera != kengine::INVALID_ID)
				g_capturedCamera = kengine::INVALID_ID;
			else {
				const auto info = kengine::cameraHelper::getViewportForPixel(window, lastMousePos);
				g_capturedCamera = info.camera;
			}

			for (const auto & [e, func] : kengine::entities.with<kengine::functions::OnMouseCaptured>())
				func(window, g_capturedCamera != kengine::INVALID_ID);
		}
	};

	kengine::pluginHelper::initPlugin(state);
	impl::init();
}
