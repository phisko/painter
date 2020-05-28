#include "Export.hpp"
#include "EntityManager.hpp"
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

static kengine::EntityManager * g_em;

#pragma region declarations
static void initKeys();
static void execute(float deltaTime);
static kengine::InputComponent CameraController(kengine::EntityManager & em);
#pragma endregion
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::pluginHelper::initPlugin(em);

	g_em = &em;

	initKeys();

	em += [&](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };
		e += CameraController(em);

		e += kengine::AdjustableComponent{
			"Camera", {
				{ "Mouse sensitivity", &MOUSE_SENSITIVITY },
				{ "Movement speed", & MOVEMENT_SPEED },
				{ "Movement speed modifier", & MOVEMENT_SPEED_MODIFIER },
				{ "Zoom speed", &ZOOM_SPEED }
			}
		};
	};
}

#pragma region initKeys
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
#pragma endregion initKeys

#pragma region Globals
static kengine::Entity::ID g_capturedCamera = kengine::Entity::INVALID_ID;
static putils::Vector3f front;
static putils::Vector3f right;
static putils::Vector3f up;
#pragma endregion Globals

#pragma region execute
#pragma region declarations
static void updateVectors(const kengine::CameraComponent & cam);
static void processKeys(putils::Point3f & pos, float deltaTime);
#pragma endregion
static void execute(float deltaTime) {
	if (g_capturedCamera == kengine::Entity::INVALID_ID)
		return;

	auto & e = g_em->getEntity(g_capturedCamera);
	auto & comp = e.get<kengine::CameraComponent>();
	static bool first = true;
	if (first) {
		updateVectors(comp);
		first = false;
	}

	auto & pos = comp.frustum.position;
	processKeys(pos, deltaTime);
}

static void processKeys(putils::Point3f & pos, float deltaTime) {
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

static void updateVectors(const kengine::CameraComponent & cam) {
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
#pragma endregion execute

#pragma region CameraController
#pragma region declarations
static void processMouseMovement(kengine::EntityManager & em, const putils::Point2f & movement);
static void processMouseScroll(kengine::EntityManager & em, float yoffset);
static void toggleMouseCapture(kengine::EntityManager & em, kengine::Entity::ID window, const putils::Point2f & lastMousePos);
#pragma endregion
static kengine::InputComponent CameraController(kengine::EntityManager & em) {
	static putils::Point2f lastMousePos;

	kengine::InputComponent input;
	input.onMouseMove = [&](kengine::Entity::ID window, const putils::Point2f & coords, const putils::Point2f & rel) {
		lastMousePos = coords;
		if (g_capturedCamera != kengine::Entity::INVALID_ID)
			processMouseMovement(em, rel);
	};
	input.onScroll = [&](kengine::Entity::ID window, float deltaX, float deltaY, const putils::Point2f & coords) {
		if (g_capturedCamera != kengine::Entity::INVALID_ID)
			processMouseScroll(em, deltaY);
	};
	input.onKey = [&em](kengine::Entity::ID window, int key, bool pressed) {
		if (pressed && key == GLFW_KEY_ENTER)
			toggleMouseCapture(em, window, lastMousePos);

		for (auto & k : keys)
			if (k.code == key)
				k.pressed = pressed;
	};

	return input;
}

static void processMouseMovement(kengine::EntityManager & em, const putils::Point2f & movement) {
	const float xoffset = movement.x * MOUSE_SENSITIVITY;
	const float yoffset = movement.y * MOUSE_SENSITIVITY;

	auto & e = em.getEntity(g_capturedCamera);
	auto & cam = e.get<kengine::CameraComponent>();

	cam.yaw -= xoffset;
	cam.pitch -= yoffset;

	const auto pitchLimit = putils::pi / 2.f - .001f;
	cam.pitch = std::min(cam.pitch, pitchLimit);
	cam.pitch = std::max(cam.pitch, -pitchLimit);

	updateVectors(cam);
}

static void processMouseScroll(kengine::EntityManager & em, float yoffset) {
	auto & e = em.getEntity(g_capturedCamera);
	auto & cam = e.get<kengine::CameraComponent>();

	auto & zoom = cam.frustum.size.y;
	if (zoom >= .001f && zoom <= putils::pi * .9f)
		zoom -= yoffset * ZOOM_SPEED;
	zoom = std::max(zoom, .001f);
	zoom = std::min(zoom, putils::pi * .9f);
}

static void toggleMouseCapture(kengine::EntityManager & em, kengine::Entity::ID window, const putils::Point2f & lastMousePos) {
	if (g_capturedCamera != kengine::Entity::INVALID_ID)
		g_capturedCamera = kengine::Entity::INVALID_ID;
	else {
		const auto info = kengine::cameraHelper::getViewportForPixel(em, window, lastMousePos);
		g_capturedCamera = info.camera;
	}

	for (const auto & [e, func] : em.getEntities<kengine::functions::OnMouseCaptured>())
		func(window, g_capturedCamera != kengine::Entity::INVALID_ID);
}
#pragma endregion CameraController
