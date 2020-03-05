#include "registerTypes.hpp"
#include "data/TimeModulatorComponent.hpp"

void registerTimeModulatorComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::TimeModulatorComponent
	>(em);
}