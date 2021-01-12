#include "helpers/registerTypeHelper.hpp"
#include "functions/Execute.hpp"

void registerExecuteComponent() {
	kengine::registerComponents<
		kengine::functions::Execute
	>();
}
