#include "CameraSystem.hpp"
#include "Export.hpp"
#include "EntityManager.hpp"
#include "vector.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/ViewportComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"

#include "packets/CaptureMouse.hpp"
#include "helpers/CameraHelper.hpp"

#include "imgui.h"
#include "GLFW/glfw3.h"
#include "magic_enum.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new CameraSystem(em);
}

static auto MOUSE_SENSITIVITY = .005f;
static auto MOVEMENT_SPEED = 10.f;
static auto MOVEMENT_SPEED_MODIFIER = 2.f;
static auto ZOOM_SPEED = .1f;

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
Key keys[putils::magic_enum::enum_count<Inputs>()];

static void initKeys() {
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

static kengine::Entity::ID g_capturedCamera = kengine::Entity::INVALID_ID;

static putils::Vector3f front;
static putils::Vector3f right;
static putils::Vector3f up;

static void updateVectors(const kengine::CameraComponent3f & cam) {
	front = {
		std::sin(cam.yaw) * std::cos(cam.pitch),
		std::sin(cam.pitch),
		std::cos(cam.yaw) * std::cos(cam.pitch)
	};
	front.normalize();

	right = putils::cross(front, { 0.f, 1.f, 0.f });
	right.normalize();

	up = putils::cross(right, front);
	up.normalize();
}

static void processMouseMovement(const putils::Point2f & movement, kengine::EntityManager & em) {
	const float xoffset = movement.x * MOUSE_SENSITIVITY;
	const float yoffset = movement.y * MOUSE_SENSITIVITY;

	auto & e = em.getEntity(g_capturedCamera);
	auto & cam = e.get<kengine::CameraComponent3f>();

	cam.yaw -= xoffset;
	cam.pitch -= yoffset;

	cam.pitch = std::min(cam.pitch, KENGINE_PI / 2.f - 0.001f);
	cam.pitch = std::max(cam.pitch, -KENGINE_PI / 2.f - 0.001f);

	updateVectors(cam);
}

void processMouseScroll(float yoffset, kengine::EntityManager & em) {
	auto & e = em.getEntity(g_capturedCamera);
	auto & cam = e.get<kengine::CameraComponent3f>();

	auto & zoom = cam.frustum.size.y;
	if (zoom >= .001f && zoom <= KENGINE_PI * .9f)
		zoom -= yoffset * ZOOM_SPEED;
	zoom = std::max(zoom, .001f);
	zoom = std::min(zoom, KENGINE_PI * .9f);
}

static auto CameraController(kengine::EntityManager & em) {
	return [&](kengine::Entity & e) {
		static putils::Point2f lastMousePos;

		kengine::InputComponent input;
		input.onMouseMove = [&](kengine::Entity::ID window, const putils::Point2f & coords, const putils::Point2f & rel) {
			lastMousePos = coords;
			if (g_capturedCamera != kengine::Entity::INVALID_ID)
				processMouseMovement(rel, em);
		};
		input.onScroll = [&](kengine::Entity::ID window, float deltaX, float deltaY, const putils::Point2f & coords) {
			if (g_capturedCamera != kengine::Entity::INVALID_ID)
				processMouseScroll(deltaY, em);
		};
		input.onKey = [&em](kengine::Entity::ID window, int key, bool pressed) {
			if (pressed && key == GLFW_KEY_ENTER) {
				if (g_capturedCamera != kengine::Entity::INVALID_ID)
					g_capturedCamera = kengine::Entity::INVALID_ID;
				else {
					const auto info = kengine::CameraHelper::getViewportForPixel(em, window, lastMousePos);
					g_capturedCamera = info.camera;
				}
				em.send(kengine::packets::CaptureMouse{ window, g_capturedCamera != kengine::Entity::INVALID_ID });
			}

			for (auto & k : keys)
				if (k.code == key)
					k.pressed = pressed;
		};

		e += input;
	};
}


CameraSystem::CameraSystem(kengine::EntityManager & em) : System(em), _em(em) {
	initKeys();

	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Mouse sensitivity", &MOUSE_SENSITIVITY); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Movement speed", &MOVEMENT_SPEED); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Movement speed modifier", &MOVEMENT_SPEED_MODIFIER); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Zoom speed", &ZOOM_SPEED); };
	_em += CameraController(_em);
}

void CameraSystem::execute() {
	if (g_capturedCamera == kengine::Entity::INVALID_ID)
		return;

	auto & e = _em.getEntity(g_capturedCamera);
	auto & comp = e.get<kengine::CameraComponent3f>();
	static bool first = true;
	if (first) {
		updateVectors(comp);
		first = false;
	}

	const auto deltaTime = time.getDeltaTime().count();
	const auto velocity = MOVEMENT_SPEED * deltaTime;

	auto & pos = comp.frustum.position;

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