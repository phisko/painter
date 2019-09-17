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

static auto ROLL_SPEED = 1.f;
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
	ROLL_LEFT,
	ROLL_RIGHT
};
bool keys[putils::magic_enum::enum_count<Inputs>()];

static putils::Point3f front;
static putils::Point3f right;
static putils::Point3f up;

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

static void processMouseMovement(float xpos, float ypos, kengine::CameraComponent3f & cam) {
	static float lastX = FLT_MAX;
	static float lastY = FLT_MAX;

	if (lastX == FLT_MAX) {
		lastX = xpos;
		lastY = ypos;
	}

	const float xoffset = (xpos - lastX) * MOUSE_SENSITIVITY;
	lastX = xpos;

	const float yoffset = (lastY - ypos) * MOUSE_SENSITIVITY;
	lastY = ypos;

	cam.yaw += xoffset;
	cam.pitch += yoffset;

	if (cam.pitch > PI / 2.f - 0.001f)
		cam.pitch = PI / 2.f - 0.001f;
	if (cam.pitch < -PI / 2.f + 0.001f)
		cam.pitch = -PI / 2.f + 0.001f;

	updateVectors(cam);
}

void processMouseScroll(float yoffset, kengine::CameraComponent3f & cam) {
	auto & zoom = cam.frustrum.size.y;

	if (zoom >= .001f && zoom <= PI * .9f)
		zoom -= yoffset * ZOOM_SPEED;
	if (zoom <= .001f)
		zoom = .001f;
	if (zoom >= PI * .9f)
		zoom = PI * .9f;
}

static void addCameraController(kengine::Entity & e, kengine::EntityManager & em) {
	static bool INPUT_ENABLED = false;

	const auto id = e.id;
	auto & comp = e.attach<kengine::InputComponent>();
	comp.onMouseMove = [id, &em](float x, float y) {
		if (INPUT_ENABLED)
			processMouseMovement(x, y, em.getEntity(id).get<kengine::CameraComponent3f>());
	};
	comp.onMouseWheel = [id, &em](float delta, float x, float y) {
		if (INPUT_ENABLED)
			processMouseScroll(delta, em.getEntity(id).get<kengine::CameraComponent3f>());
	};
	comp.onKey = [](int key, bool pressed) {
		static const auto associations = putils::make_vector(
			std::make_pair('W', FORWARD),
			std::make_pair('S', BACKWARD),
			std::make_pair('A', LEFT),
			std::make_pair('D', RIGHT),
			std::make_pair('R', UP),
			std::make_pair('F', DOWN),
			std::make_pair('Q', ROLL_LEFT),
			std::make_pair('E', ROLL_RIGHT)
		);

		if (pressed) {
			if (key == GLFW_KEY_LEFT_CONTROL)
				MOVEMENT_SPEED /= MOVEMENT_SPEED_MODIFIER;
			else if (key == GLFW_KEY_LEFT_SHIFT)
				MOVEMENT_SPEED *= MOVEMENT_SPEED_MODIFIER;
			else if (key == GLFW_KEY_ENTER)
				INPUT_ENABLED = !INPUT_ENABLED;
		}

		for (const auto & p : associations)
			if (key == p.first)
				keys[p.second] = pressed;
	};
}

CameraSystem::CameraSystem(kengine::EntityManager & em) : System(em), _em(em) {
	onLoad("");
}

void CameraSystem::onLoad(const char *) noexcept {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Mouse sensitivity", &MOUSE_SENSITIVITY); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Movement speed", &MOVEMENT_SPEED); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Movement speed modifier", &MOVEMENT_SPEED_MODIFIER); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Zoom speed", &ZOOM_SPEED); };
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Camera] Roll speed", &ROLL_SPEED); };
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

		if (keys[FORWARD])
			pos += front * velocity;
		if (keys[BACKWARD])
			pos -= front * velocity;
		if (keys[LEFT])
			pos -= right * velocity;
		if (keys[RIGHT])
			pos += right * velocity;
		if (keys[UP])
			pos += up * velocity;
		if (keys[DOWN])
			pos -= up * velocity;

		if (keys[ROLL_LEFT])
			comp.roll += ROLL_SPEED * deltaTime;
		if (keys[ROLL_RIGHT])
			comp.roll -= ROLL_SPEED * deltaTime;
	}
}
