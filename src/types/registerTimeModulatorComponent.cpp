#include "helpers/registerTypeHelper.hpp"
#include "data/TimeModulatorComponent.hpp"

void registerTimeModulatorComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::TimeModulatorComponent
	>(em);
}
