#include "registerTypes.hpp"
#include "data/AdjustableComponent.hpp"

void registerAdjustableComponent(kengine::EntityManager & em) {
	registerComponents<
		kengine::AdjustableComponent
	>(em);

	registerTypes<
		kengine::AdjustableComponent::Value,
		kengine::AdjustableComponent::Value::BoolStorage, 
		kengine::AdjustableComponent::Value::FloatStorage,
		kengine::AdjustableComponent::Value::IntStorage,
		kengine::AdjustableComponent::Value::ColorStorage
	>(em);
}