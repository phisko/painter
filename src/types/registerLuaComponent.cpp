#include "registerTypes.hpp"
#include "data/LuaComponent.hpp"
#include "data/LuaTableComponent.hpp"

void registerLuaComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::LuaComponent,
		kengine::LuaTableComponent
	>(em);
}
