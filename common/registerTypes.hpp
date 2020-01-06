#pragma once

#include <string>
#include "EntityManager.hpp"

#include "systems/LuaSystem.hpp"
#include "systems/PySystem.hpp"

#include "helpers/RegisterComponentFunctions.hpp"
#include "helpers/RegisterComponentEditor.hpp"
#include "helpers/RegisterComponentMatcher.hpp"
#include "helpers/RegisterComponentJSONLoader.hpp"
#include "helpers/RegisterComponentEntityIterators.hpp"
#include "data/NameComponent.hpp"

#include "systems/LuaSystem.hpp"

#include "Point.hpp"

template<typename F>
static void registerFunction(kengine::EntityManager & em, const std::string & name, F && func) {
	kengine::lua::registerFunction(em, name, func);
	kengine::python::registerFunction(em, name, func);
}

template<typename ...Types>
static void registerTypes(kengine::EntityManager & em) {
	putils::for_each_type<Types...>([&](auto && t) {
		using T = putils_wrapped_type(t);
		kengine::lua::registerType<T>(em);
		kengine::python::registerType<T>(em);
	});
}

template<typename ... Comps>
static void registerComponents(kengine::EntityManager & em) {
	registerTypes<Comps...>(em);

	putils::for_each_type<Comps...>([&](auto && t) {
		using T = putils_wrapped_type(t);
		kengine::TypeHelper::getTypeEntity<T>(em) += kengine::NameComponent{ putils::reflection::get_class_name<T>() };
	});

	kengine::registerComponentsFunctions<Comps...>(em);
	kengine::registerComponentEditors<Comps...>(em);
	kengine::registerComponentMatchers<Comps...>(em);
	kengine::registerComponentJSONLoaders<Comps...>(em);
	kengine::registerComponentEntityIterators<Comps...>(em);
}
