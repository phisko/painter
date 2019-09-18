#include "registerTypes.h"

void registerComponents1(kengine::EntityManager & em);
void registerComponents2(kengine::EntityManager & em);
void registerComponents3(kengine::EntityManager & em);

void registerTypes(kengine::EntityManager & em) {
	registerComponents1(em);
	registerComponents2(em);
	registerComponents3(em);

	registerTypes<
		kengine::LuaComponent::script, kengine::LuaComponent::script_vector,
		putils::Rect3f, putils::Point3f,
		putils::Color, putils::NormalizedColor
	>(em);

	if constexpr (!std::is_same<kengine::LuaComponent::script, kengine::PyComponent::script>::value)
		registerTypes<kengine::PyComponent::script>(em);

	if constexpr (!std::is_same<kengine::LuaComponent::script_vector, kengine::PyComponent::script_vector>::value)
		registerTypes<kengine::PyComponent::script_vector>(em);
}
