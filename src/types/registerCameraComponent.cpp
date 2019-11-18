#include "registerTypes.hpp"
#include "components/CameraComponent.hpp"
#include "components/ViewportComponent.hpp"
#include "components/WindowComponent.hpp"

void registerCameraComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::CameraComponent3f,
		kengine::ViewportComponent,
		kengine::WindowComponent
	>(em);
}
