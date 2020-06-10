#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"

void registerPythonComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::PythonComponent
	>(em);
}
