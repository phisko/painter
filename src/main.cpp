#include <thread>

#include "go_to_bin_dir.hpp"
#include "PluginManager.hpp"
#include "kengine.hpp"

#include "helpers/commandLineHelper.hpp"
#include "helpers/mainLoop.hpp"
#include "helpers/imguiLuaHelper.hpp"

#include "systems/input/InputSystem.hpp"
#include "systems/lua/LuaSystem.hpp"
#include "systems/python/PythonSystem.hpp"
#include "systems/onclick/OnClickSystem.hpp"

#include "systems/imgui_adjustable/ImGuiAdjustableSystem.hpp"
#include "systems/imgui_tool/ImGuiToolSystem.hpp"
#include "systems/imgui_engine_stats/ImGuiEngineStatsSystem.hpp"
#include "systems/imgui_entity_editor/ImGuiEntityEditorSystem.hpp"
#include "systems/imgui_entity_selector/ImGuiEntitySelectorSystem.hpp"
#include "systems/imgui_prompt/ImGuiPromptSystem.hpp"

#include "systems/log_imgui/LogImGuiSystem.hpp"
#include "systems/log_visual_studio/LogVisualStudio.hpp"

#include "systems/model_creator/ModelCreatorSystem.hpp"
#include "systems/polyvox/PolyVoxSystem.hpp"
#include "systems/polyvox/MagicaVoxelSystem.hpp"
#include "systems/recast/RecastSystem.hpp"
#include "systems/glfw/GLFWSystem.hpp"
#include "systems/kreogl/KreoglSystem.hpp"

#include "systems/bullet/BulletSystem.hpp"
#include "systems/kinematic/KinematicSystem.hpp"

#include "data/WindowComponent.hpp"

int main(int ac, const char ** av) {
	putils::goToBinDir(av[0]);

#if defined(_WIN32) && defined(KENGINE_NDEBUG)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	kengine::init(std::thread::hardware_concurrency());

	kengine::createCommandLineEntity(ac, av);

	kengine::entities += kengine::LogImGuiSystem();
	kengine::entities += kengine::LogVisualStudioSystem();

	kengine::entities += kengine::LuaSystem();
	kengine::entities += kengine::PythonSystem();

	kengine::entities += [&](kengine::Entity & e) {
		e += kengine::WindowComponent{
			"Painter"
		};
	};

	kengine::entities += kengine::InputSystem();
	kengine::entities += kengine::OnClickSystem();

	kengine::entities += kengine::ModelCreatorSystem();
    kengine::entities += kengine::KreoglSystem();
	kengine::entities += kengine::GLFWSystem();
	kengine::entities += kengine::PolyVoxSystem();
	kengine::entities += kengine::MagicaVoxelSystem();

	kengine::entities += kengine::BulletSystem();
	kengine::entities += kengine::KinematicSystem();
	kengine::entities += kengine::RecastSystem();

	kengine::entities += kengine::ImGuiAdjustableSystem();
	kengine::entities += kengine::ImGuiToolSystem();
	kengine::entities += kengine::ImGuiEngineStatsSystem();
	kengine::entities += kengine::ImGuiEntityEditorSystem();
	kengine::entities += kengine::ImGuiEntitySelectorSystem();
	kengine::entities += kengine::ImGuiPromptSystem();

	kengine::types::registerTypes();

	putils::PluginManager pm;
	pm.rescanDirectory("plugins", "loadKenginePlugin", kengine::getState());

	kengine::imguiLuaHelper::initBindings();

	kengine::mainLoop::timeModulated::run();

	return 0;
}
