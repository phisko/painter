#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"

void registerPythonComponent() {
	kengine::registerComponents<
		kengine::PythonComponent
	>();
}
