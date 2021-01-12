#include "helpers/registerTypeHelper.hpp"
#include "functions/AppearsInViewport.hpp"

void registerAppearsInViewportComponent() {
	kengine::registerComponents<
		kengine::functions::AppearsInViewport
	>();
}
