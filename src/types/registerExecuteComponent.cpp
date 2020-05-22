#include "helpers/RegisterTypeHelper.hpp"
#include "functions/Execute.hpp"

void registerExecuteComponent(kengine::EntityManager & em) {
	kengine::registerComponents<
		kengine::functions::Execute
	>(em);
}
