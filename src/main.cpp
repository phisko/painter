#include "go_to_bin_dir.hpp"
#include "PluginManager.hpp"

#include "helpers/MainLoop.hpp"

#include "systems/InputSystem.hpp"
#include "systems/LuaSystem.hpp"
#include "systems/PySystem.hpp"
#include "systems/OnClickSystem.hpp"

#include "systems/ImGuiAdjustableSystem.hpp"
#include "systems/ImGuiToolSystem.hpp"
#include "systems/ImGuiEntityEditorSystem.hpp"
#include "systems/ImGuiEntitySelectorSystem.hpp"

#include "systems/polyvox/PolyVoxSystem.hpp"
#include "systems/polyvox/MagicaVoxelSystem.hpp"
#include "systems/assimp/AssimpSystem.hpp"
#include "systems/opengl/OpenGLSystem.hpp"
#include "systems/opengl_sprites/OpenGLSpritesSystem.hpp"

#include "systems/bullet/BulletSystem.hpp"
#include "systems/KinematicSystem.hpp"

#include "data/WindowComponent.hpp"

int main(int, char **av) {
	putils::goToBinDir(av[0]);

#if defined(_WIN32) && defined(NDEBUG)
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

	em += kengine::ImGuiAdjustableSystem(em);
	em += kengine::ImGuiToolSystem(em);
	em += kengine::ImGuiEntityEditorSystem(em);
	em += kengine::ImGuiEntitySelectorSystem(em);

	putils::PluginManager pm;
	pm.rescanDirectory("plugins", "loadKenginePlugin", em);

	extern void registerTypes(kengine::EntityManager &);
	registerTypes(em);

	kengine::MainLoop::run(em);

	return 0;
}
