#include "registerTypes.hpp"
#include "components/ModelComponent.hpp"

void registerModelComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::ModelComponent
	>(em);
}
