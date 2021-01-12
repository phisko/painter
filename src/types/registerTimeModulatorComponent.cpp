#include "helpers/registerTypeHelper.hpp"
#include "data/TimeModulatorComponent.hpp"

void registerTimeModulatorComponent() {
	kengine::registerComponents<
		kengine::TimeModulatorComponent
	>();
}
