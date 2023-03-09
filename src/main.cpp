// entt
#include <entt/entity/registry.hpp>

// pybind11
#include <pybind11/pybind11.h>

// putils
#include "putils/thread_name.hpp"
#include "putils/plugin_manager/plugin_manager.hpp"

// kengine
#include "kengine/config/imgui/systems/system.hpp"
#include "kengine/config/json/systems/system.hpp"
#include "kengine/async/helpers/start_task.hpp"
#include "kengine/async/helpers/process_results.hpp"
#include "kengine/async/imgui/systems/system.hpp"
#include "kengine/command_line/helpers/create_entity.hpp"
#include "kengine/imgui/tool/systems/system.hpp"
#include "kengine/input/systems/system.hpp"
#include "kengine/json_scene_loader/data/request.hpp"
#include "kengine/json_scene_loader/systems/system.hpp"
#include "kengine/core/log/file/systems/system.hpp"
#include "kengine/core/log/imgui/systems/system.hpp"
#include "kengine/core/log/standard_output/systems/system.hpp"
#include "kengine/core/log/visual_studio/systems/system.hpp"
#include "kengine/main_loop/data/keep_alive.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/run.hpp"
#include "kengine/meta/helpers/register_all_types.hpp"
#include "kengine/meta/imgui/engine_stats/systems/system.hpp"
#include "kengine/meta/imgui/entity_editor/systems/system.hpp"
#include "kengine/meta/imgui/entity_selector/systems/system.hpp"
#include "kengine/model/find/by_name/systems/system.hpp"
#include "kengine/pathfinding/recast/systems/system.hpp"
#include "kengine/physics/bullet/systems/system.hpp"
#include "kengine/physics/kinematic/systems/system.hpp"
#include "kengine/render/data/window.hpp"
#include "kengine/render/find_model_by_asset/systems/system.hpp"
#include "kengine/render/glfw/systems/system.hpp"
#include "kengine/render/kreogl/systems/system.hpp"
#include "kengine/render/on_click/systems/system.hpp"
#include "kengine/render/polyvox/systems/system.hpp"
#include "kengine/render/polyvox/magica_voxel/systems/system.hpp"
#include "kengine/scripting/imgui_prompt/systems/system.hpp"
#include "kengine/scripting/lua/data/scripts.hpp"
#include "kengine/scripting/lua/systems/system.hpp"
#include "kengine/scripting/python/systems/system.hpp"
#include "kengine/system_creator/helpers/create_all_systems.hpp"

// types
#include "kengine/type_registration/add_type_registrator.hpp"
#include "painter/types/add_type_registrator.hpp"

static constexpr auto log_category = "main";

static void create_log_systems(entt::registry & r) noexcept {
	KENGINE_PROFILING_SCOPE;

	kengine::core::log::standard_output::add_system(r);
#ifdef _WIN32
	kengine::core::log::visual_studio::add_system(r);
#endif
	kengine::core::log::file::add_system(r);
	kengine::core::log::imgui::add_system(r);
}

static void create_objects(entt::registry & r) noexcept {
	KENGINE_PROFILING_SCOPE;
	kengine_log(r, log, log_category, "Creating objects");

	const auto e = r.create();
	r.emplace<kengine::render::window>(e, "Painter");
	r.emplace<kengine::main_loop::keep_alive>(e);

	const auto scene_loader = r.create();
	r.emplace<kengine::json_scene_loader::request>(
		scene_loader,
		kengine::json_scene_loader::request{
			.temporary_scene = { "resources/loading_scene.json" },
			.model_directory = { "resources/models" },
			.scene = { "resources/scene.json" },
		}
	);
}

static void setup_systems(entt::registry & r) noexcept {
	KENGINE_PROFILING_SCOPE;

	kengine_log(r, log, log_category, "Adding script language systems");
	// Create the script language systems immediately (so the async type registration can access their states)
	kengine::scripting::python::add_system(r);
	kengine::scripting::lua::add_system(r);

	kengine_log(r, log, log_category, "Pre-registering input systems");
	// Pre-register all other systems (so all component storages are instantiated before we start the async tasks)
	kengine::input::register_system(r);
	kengine::render::on_click::register_system(r);

	kengine_log(r, log, log_category, "Pre-registering graphics systems");
	kengine::model::find::by_name::register_system(r);
	kengine::render::find_model_by_asset::register_system(r);
	kengine::render::kreogl::register_system(r);
	kengine::render::glfw::register_system(r);
	kengine::render::polyvox::register_system(r);
	kengine::render::polyvox::magica_voxel::register_system(r);

	kengine_log(r, log, log_category, "Pre-registering gameplay systems");
	kengine::physics::bullet::register_system(r);
	kengine::physics::kinematic::register_system(r);
	kengine::pathfinding::recast::register_system(r);

	kengine_log(r, log, log_category, "Pre-registering imgui systems");
	kengine::config::imgui::register_system(r);
	kengine::async::imgui::register_system(r);
	kengine::imgui::tool::register_system(r);
	kengine::meta::imgui::engine_stats::register_system(r);
	kengine::meta::imgui::entity_editor::register_system(r);
	kengine::meta::imgui::entity_selector::register_system(r);
	kengine::scripting::imgui_prompt::register_system(r);
}

static void start_type_registration_thread(entt::registry & r) noexcept {
	kengine::types::add_type_registrator(r);
	painter::types::add_type_registrator(r);
	kengine::meta::pre_register_all_types(r);

	// Release the Python state so that the type registration thread can use it
	// Make it an optional since it has to be destroyed before the Python state (which will be destroyed by `r.clear()` at the end of main)
	// Make it static as it doesn't support move semantics (its dtor will just freeze after being moved from)
	// So we can't:
	//		* move it to the `main_loop::execute` lambda
	//		* hold it as a component, since that also calls a move ctor
	static std::optional<pybind11::gil_scoped_release> python_release;
	python_release.emplace();

	struct type_registration_result {};
	const auto registrator = r.create();

	kengine::async::start_task(
		r, registrator,
		kengine::async::task::string("register types"),
		std::async(std::launch::async, [&r] {
			const putils::scoped_thread_name thread_name("Register types");
			kengine::meta::register_all_types(r);
			return type_registration_result{};
		})
	);

	r.emplace<kengine::main_loop::execute>(registrator, [&r, registrator](float delta_time) {
		const auto done = kengine::async::process_results<type_registration_result>(r);
		if (!done)
			return;

		// Load plugins before the scene loader (since they might register types)
		kengine_log(r, log, log_category, "Loading plugins");
		putils::plugin_manager pm;
		pm.rescan_directory("plugins", "load_kengine_plugin", r);

		// Re-acquire the Python state (since we'll be executing scripts)
		kengine_log(r, verbose, log_category, "Re-acquiring Python GIL");
		python_release.reset();

		// Only create these now, as they require meta components
		kengine_log(r, log, log_category, "Adding loading systems");
		kengine::config::json::add_system(r);
		kengine::json_scene_loader::add_system(r);

		// Do this last, as it will destroy this functor (and invalidate our reference to `r`)
		r.destroy(registrator);
	});
}

int main(int ac, const char ** av) {
	entt::registry r;
	kengine::command_line::create_entity(r, ac, av);

	// Create the log systems immediately (to get as much log as possible)
	create_log_systems(r);

	// Go to executable directory to be near "resources"
	const auto bin_dir = std::filesystem::path(av[0]).parent_path();
	kengine_logf(r, verbose, log_category, "Changing working directory to {}", bin_dir.string());
	if (exists(bin_dir))
		std::filesystem::current_path(bin_dir);

#if defined(_WIN32) && defined(KENGINE_NDEBUG)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	create_objects(r);
	setup_systems(r);
	start_type_registration_thread(r);

	kengine::system_creator::create_all_systems(r);
	kengine::main_loop::time_modulated::run(r);

	// Explicitly clear so that component dtors are called before pools are invalidated
	kengine_log(r, log, log_category, "Clearing registry");
	r.clear();
	return 0;
}