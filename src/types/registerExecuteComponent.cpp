#include "registerTypes.hpp"
#include "functions/Execute.hpp"

void registerExecuteComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::functions::Execute
	>(em);
}