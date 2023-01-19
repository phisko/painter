#include "plugin_manager.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/keep_alive.hpp"
#include "kengine/data/lua.hpp"
#include "kengine/data/python.hpp"
#include "kengine/data/window.hpp"

// kengine systems
#include "kengine/systems/bullet/bullet.hpp"
#include "kengine/systems/glfw/glfw.hpp"
#include "kengine/systems/imgui_adjustable/imgui_adjustable.hpp"
#include "kengine/systems/imgui_engine_stats/imgui_engine_stats.hpp"
#include "kengine/systems/imgui_entity_editor/imgui_entity_editor.hpp"
#include "kengine/systems/imgui_entity_selector/imgui_entity_selector.hpp"
#include "kengine/systems/imgui_prompt/imgui_prompt.hpp"
#include "kengine/systems/imgui_tool/imgui_tool.hpp"
#include "kengine/systems/input/input.hpp"
#include "kengine/systems/kinematic/kinematic.hpp"
#include "kengine/systems/kreogl/kreogl.hpp"
#include "kengine/systems/log_file/log_file.hpp"
#include "kengine/systems/log_imgui/log_imgui.hpp"
#include "kengine/systems/log_stdout/log_stdout.hpp"
#include "kengine/systems/log_visual_studio/log_visual_studio.hpp"
#include "kengine/systems/lua/lua.hpp"
#include "kengine/systems/model_creator/model_creator.hpp"
#include "kengine/systems/on_click/on_click.hpp"
#include "kengine/systems/polyvox/magica_voxel.hpp"
#include "kengine/systems/polyvox/polyvox.hpp"
#include "kengine/systems/python/python.hpp"
#include "kengine/systems/recast/recast.hpp"

// kengine helpers
#include "kengine/helpers/command_line_helper.hpp"
#include "kengine/helpers/imgui_lua_helper.hpp"
#include "kengine/helpers/main_loop.hpp"
#include "kengine/helpers/register_all_types.hpp"

// types
#include "kengine/types/add_type_registrator.hpp"
#include "painter/types/add_type_registrator.hpp"

int main(int ac, const char ** av) {
	// Go to executable directory to be near "resources"
	const auto bin_dir = std::filesystem::path(av[0]).parent_path();
	if (exists(bin_dir))
		std::filesystem::current_path(bin_dir);

#if defined(_WIN32) && defined(KENGINE_NDEBUG)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	entt::registry r;

	kengine::create_command_line_entity(r, ac, av);

	kengine::systems::add_log_file(r);
	kengine::systems::add_log_imgui(r);
	kengine::systems::add_log_stdout(r);
#ifdef _WIN32
	kengine::systems::add_log_visual_studio(r);
#endif

	kengine::systems::add_lua(r);
	kengine::systems::add_python(r);

	const auto e = r.create();
	r.emplace<kengine::data::window>(e, "Painter");
	r.emplace<kengine::data::keep_alive>(e);
	r.emplace<kengine::data::lua>(e, kengine::data::lua{ { "scripts/test.lua" } });
	r.emplace<kengine::data::python>(e, kengine::data::python{ { "scripts/test.py" } });

	kengine::systems::add_input(r);
	kengine::systems::add_on_click(r);

	kengine::systems::add_model_creator(r);
	kengine::systems::add_kreogl(r);
	kengine::systems::add_glfw(r);
	kengine::systems::add_polyvox(r);
	kengine::systems::add_magica_voxel(r);

	kengine::systems::add_bullet(r);
	kengine::systems::add_kinematic(r);
	kengine::systems::add_recast(r);

	kengine::systems::add_imgui_adjustable(r);
	kengine::systems::add_imgui_tool(r);
	kengine::systems::add_imgui_engine_stats(r);
	kengine::systems::add_imgui_entity_editor(r);
	kengine::systems::add_imgui_entity_selector(r);
	kengine::systems::add_imgui_prompt(r);

	kengine::types::add_type_registrator(r);
	painter::types::add_type_registrator(r);
	kengine::register_all_types(r);

	putils::plugin_manager pm;
	pm.rescan_directory("plugins", "load_kengine_plugin", r);

	kengine::imgui_lua_helper::init_bindings(r);

	kengine::main_loop::time_modulated::run(r);

	r.clear(); // Explicitly clear so that component dtors are called before pools are invalidated
	return 0;
}