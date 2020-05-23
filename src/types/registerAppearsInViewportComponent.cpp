#include "helpers/registerTypeHelper.hpp"
#include "functions/AppearsInViewport.hpp"

void registerAppearsInViewportComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::functions::AppearsInViewport
	>(em);
}
