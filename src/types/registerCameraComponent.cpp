#include "registerTypes.hpp"
#include "components/CameraComponent.hpp"

void registerCameraComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::CameraComponent3f
	>(em);
}
