#include "CameraSystem.hpp"
#include "Export.hpp"
#include "EntityManager.hpp"
#include "vector.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"

#include "imgui.h"
#include "GLFW/glfw3.h"
#include "magic_enum.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new CameraSystem(em);
}

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

CameraSystem::CameraSystem(kengine::EntityManager & em) : System(em), _em(em) {
	onLoad("");
}

static auto MOUSE_SENSITIVITY = .005f;
static auto MOVEMENT_SPEED = 10.f;
static auto MOVEMENT_SPEED_MODIFIER = 2.f;
static auto ZOOM_SPEED = .1f;

void CameraSystem::onLoad(const char *) noexcept {
	initKeys();

	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Mouse sensitivity", &MOUSE_SENSITIVITY); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Movement speed", &MOVEMENT_SPEED); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Movement speed modifier", &MOVEMENT_SPEED_MODIFIER); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Zoom speed", &ZOOM_SPEED); };
}

static putils::Vector3f front;
static putils::Vector3f right;
static putils::Vector3f up;

static void updateVectors(const kengine::CameraComponent3f & cam) {
	front = {
		std::cos(cam.yaw) * std::cos(cam.pitch),
		std::sin(cam.pitch),
		std::sin(cam.yaw) * std::cos(cam.pitch)
	};
	front.normalize();

	right = putils::cross(front, { 0.f, 1.f, 0.f });
	right.normalize();

	up = putils::cross(right, front);
	up.normalize();
}

static void processMouseMovement(float xrel, float yrel, kengine::CameraComponent3f & cam) {
	const float xoffset = xrel * MOUSE_SENSITIVITY;
	const float yoffset = yrel * MOUSE_SENSITIVITY;

	cam.yaw += xoffset;
	cam.pitch += yoffset;

	cam.pitch = std::min(cam.pitch, KENGINE_PI / 2.f - 0.001f);
	cam.pitch = std::max(cam.pitch, -KENGINE_PI / 2.f - 0.001f);

	updateVectors(cam);
}

void processMouseScroll(float yoffset, kengine::CameraComponent3f & cam) {
	auto & zoom = cam.frustrum.size.y;
	if (zoom >= .001f && zoom <= KENGINE_PI * .9f)
		zoom -= yoffset * ZOOM_SPEED;
	zoom = std::max(zoom, .001f);
	zoom = std::min(zoom, KENGINE_PI * .9f);
}

static void addCameraController(kengine::Entity & e, kengine::EntityManager & em) {
	static bool MOUSE_CAPTURED = false;

	const auto id = e.id;
	kengine::InputComponent input;

	input.onMouseMove = [id, &em](float x, float y, float xrel, float yrel) {
		if (MOUSE_CAPTURED)
			processMouseMovement(xrel, yrel, em.getEntity(id).get<kengine::CameraComponent3f>());
	};
	input.onMouseWheel = [id, &em](float delta, float x, float y) {
		if (MOUSE_CAPTURED)
			processMouseScroll(delta, em.getEntity(id).get<kengine::CameraComponent3f>());
	};
	input.onKey = [](int key, bool pressed) {
		if (pressed && key == GLFW_KEY_ENTER)
			MOUSE_CAPTURED = !MOUSE_CAPTURED;

		for (auto & k : keys)
			if (k.code == key)
				k.pressed = pressed;
	};

	e += input;
}

void CameraSystem::execute() {
	for (auto & [e, comp] : _em.getEntities<kengine::CameraComponent3f>()) {
		if (!e.has<kengine::InputComponent>()) {
			addCameraController(e, _em);
			updateVectors(comp);
		}

		const auto deltaTime = time.getDeltaTime().count();
		const auto velocity = MOVEMENT_SPEED * deltaTime;

		auto & pos = comp.frustrum.position;

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
}
