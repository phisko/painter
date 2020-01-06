#include "registerTypes.hpp"
#include "data/CameraComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"

void registerCameraComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::CameraComponent,
		kengine::ViewportComponent,
		kengine::WindowComponent
	>(em);
}
