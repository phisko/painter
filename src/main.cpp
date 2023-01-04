#include "plugin_manager.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine types
#include "kengine/types/register_types.hpp"

// kengine helpers
#include "kengine/helpers/command_line_helper.hpp"
#include "kengine/helpers/main_loop.hpp"
#include "kengine/helpers/imgui_lua_helper.hpp"

// kengine input systems
#include "kengine/systems/input/input.hpp"
#include "kengine/systems/lua/lua.hpp"
#include "kengine/systems/python/python.hpp"
#include "kengine/systems/on_click/on_click.hpp"

// kengine imgui systems
#include "kengine/systems/imgui_adjustable/imgui_adjustable.hpp"
#include "kengine/systems/imgui_tool/imgui_tool.hpp"
#include "kengine/systems/imgui_engine_stats/imgui_engine_stats.hpp"
#include "kengine/systems/imgui_entity_editor/imgui_entity_editor.hpp"
#include "kengine/systems/imgui_entity_selector/imgui_entity_selector.hpp"
#include "kengine/systems/imgui_prompt/imgui_prompt.hpp"

// kengine log systems
#include "kengine/systems/log_file/log_file.hpp"
#include "kengine/systems/log_imgui/log_imgui.hpp"
#include "kengine/systems/log_stdout/log_stdout.hpp"
#include "kengine/systems/log_visual_studio/log_visual_studio.hpp"

// kengine rendering systems
#include "kengine/systems/model_creator/model_creator.hpp"
#include "kengine/systems/polyvox/polyvox.hpp"
#include "kengine/systems/polyvox/magica_voxel.hpp"
#include "kengine/systems/glfw/glfw.hpp"
#include "kengine/systems/kreogl/kreogl.hpp"

// kengine game systems
#include "kengine/systems/bullet/bullet.hpp"
#include "kengine/systems/kinematic/kinematic.hpp"
#include "kengine/systems/recast/recast.hpp"

// kengine data
#include "kengine/data/keep_alive.hpp"
#include "kengine/data/lua.hpp"
#include "kengine/data/python.hpp"
#include "kengine/data/window.hpp"

int main(int ac, const char ** av) {
	// Go to executable directory to be near "resources"
	const auto bin_dir = std::filesystem::path(av[0]).parent_path();
	if (exists(bin_dir))
		std::filesystem::current_path(bin_dir);

#if defined(_WIN32) && defined(KENGINE_NDEBUG)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	entt::registry registry;

	kengine::create_command_line_entity(registry, ac, av);

	kengine::systems::add_log_file(registry);
	kengine::systems::add_log_imgui(registry);
	kengine::systems::add_log_stdout(registry);
#ifdef _WIN32
	kengine::systems::add_log_visual_studio(registry);
#endif

	kengine::systems::add_lua(registry);
	kengine::systems::add_python(registry);

	const auto e = registry.create();
	registry.emplace<kengine::data::window>(e, "Painter");
	registry.emplace<kengine::data::keep_alive>(e);
	registry.emplace<kengine::data::lua>(e, kengine::data::lua{ { "scripts/test.lua" } });
	registry.emplace<kengine::data::python>(e, kengine::data::python{ { "scripts/test.py" } });

	kengine::systems::add_input(registry);
	kengine::systems::add_on_click(registry);

	kengine::systems::add_model_creator(registry);
	kengine::systems::add_kreogl(registry);
	kengine::systems::add_glfw(registry);
	kengine::systems::add_polyvox(registry);
	kengine::systems::add_magica_voxel(registry);

	kengine::systems::add_bullet(registry);
	kengine::systems::add_kinematic(registry);
	kengine::systems::add_recast(registry);

	kengine::systems::add_imgui_adjustable(registry);
	kengine::systems::add_imgui_tool(registry);
	kengine::systems::add_imgui_engine_stats(registry);
	kengine::systems::add_imgui_entity_editor(registry);
	kengine::systems::add_imgui_entity_selector(registry);
	kengine::systems::add_imgui_prompt(registry);

	kengine::types::register_types(registry);

	putils::plugin_manager pm;
	pm.rescan_directory("plugins", "load_kengine_plugin", registry);

	kengine::imgui_lua_helper::init_bindings(registry);

	kengine::main_loop::time_modulated::run(registry);

	registry.clear(); // Explicitly clear so that component dtors are called before pools are invalidated
	return 0;
}