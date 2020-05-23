#include "helpers/registerTypeHelper.hpp"
#include "data/CameraComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"

void registerCameraComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::CameraComponent,
		kengine::ViewportComponent,
		kengine::WindowComponent
	>(em);
}
