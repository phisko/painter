#include <thread>

#include "go_to_bin_dir.hpp"
#include "PluginManager.hpp"
#include "EntityManager.hpp"

#include "helpers/mainLoop.hpp"
#include "helpers/imguiLuaHelper.hpp"

#include "systems/InputSystem.hpp"
#include "systems/LuaSystem.hpp"
#include "systems/PySystem.hpp"
#include "systems/OnClickSystem.hpp"

#include "systems/ImGuiAdjustableSystem.hpp"
#include "systems/ImGuiToolSystem.hpp"
#include "systems/ImGuiEntityEditorSystem.hpp"
#include "systems/ImGuiEntitySelectorSystem.hpp"
#include "systems/ImGuiPromptSystem.hpp"

#include "systems/polyvox/PolyVoxSystem.hpp"
#include "systems/polyvox/MagicaVoxelSystem.hpp"
#include "systems/assimp/AssimpSystem.hpp"
#include "systems/opengl/OpenGLSystem.hpp"
#include "systems/opengl_sprites/OpenGLSpritesSystem.hpp"
#include "systems/recast/RecastSystem.hpp"

#include "systems/bullet/BulletSystem.hpp"
#include "systems/KinematicSystem.hpp"

#include "data/WindowComponent.hpp"
#include "data/LuaComponent.hpp"
#include "data/PyComponent.hpp"

int main(int, char **av) {
	putils::goToBinDir(av[0]);

#if defined(_WIN32) && defined(KENGINE_NDEBUG)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	kengine::EntityManager em(std::thread::hardware_concurrency());

	em += [&](kengine::Entity & e) {
		e += kengine::WindowComponent{
			"Painter"
		};
	};

	em += kengine::InputSystem(em);
	em += kengine::LuaSystem(em);
	em += kengine::PySystem(em);
	
	em += kengine::OnClickSystem(em);

	em += kengine::OpenGLSystem(em);
	em += kengine::OpenGLSpritesSystem(em);
	em += kengine::PolyVoxSystem(em);
	em += kengine::MagicaVoxelSystem(em);
	em += kengine::AssImpSystem(em);

	em += kengine::BulletSystem(em);
	em += kengine::KinematicSystem(em);
	em += kengine::RecastSystem(em);

	em += kengine::ImGuiAdjustableSystem(em);
	em += kengine::ImGuiToolSystem(em);
	em += kengine::ImGuiEntityEditorSystem(em);
	em += kengine::ImGuiEntitySelectorSystem(em);
	em += kengine::ImGuiPromptSystem(em);

	putils::PluginManager pm;
	pm.rescanDirectory("plugins", "loadKenginePlugin", em);

	extern void registerTypes(kengine::EntityManager &);
	registerTypes(em);

	kengine::imguiLuaHelper::initBindings(em);

	kengine::mainLoop::timeModulated::run(em);

	return 0;
}
