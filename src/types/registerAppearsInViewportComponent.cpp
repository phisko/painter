#include "registerTypes.hpp"
#include "functions/AppearsInViewport.hpp"

void registerAppearsInViewportComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::functions::AppearsInViewport
	>(em);
}