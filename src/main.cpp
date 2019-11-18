#include "go_to_bin_dir.hpp"

#include "systems/LuaSystem.hpp"
#include "systems/PySystem.hpp"
#include "systems/BehaviorSystem.hpp"
#include "systems/OnClickSystem.hpp"
#include "systems/PhysicsSystem.hpp"
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

#include "components/ImGuiComponent.hpp"
#include "components/WindowComponent.hpp"
#include "imgui.h"

#include "packets/GBuffer.hpp"
#include "packets/Terminate.hpp"

auto TimeControls(kengine::EntityManager & em) {
	return [&em](kengine::Entity & e) {
		auto & tool = e.attach<kengine::ImGuiToolComponent>();
		tool.name = "Time controller";
		tool.enabled = false;

		e += kengine::ImGuiComponent([&] {
			if (!tool.enabled)
				return;

			if (ImGui::Begin("Time controls", &tool.enabled)) {
				float speed = em.getSpeed();
				ImGui::InputFloat("Speed", &speed);
				em.setSpeed(speed);

				ImGui::Columns(2);
				if (ImGui::Button("Pause"))
					em.pause();
				ImGui::NextColumn();
				if (ImGui::Button("Resume"))
					em.resume();
				ImGui::Columns();

			} ImGui::End();
		});
	};
}

static void addMenus(kengine::EntityManager & em) {
	em += TimeControls(em);
}

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

	em.loadSystems<
		kengine::LuaSystem, kengine::PySystem,
		kengine::BehaviorSystem,
		kengine::OnClickSystem,

		kengine::OpenGLSystem,
		kengine::OpenGLSpritesSystem,
		kengine::PolyVoxSystem, kengine::MagicaVoxelSystem,
		kengine::AssImpSystem,

		kengine::BulletSystem,

		kengine::ImGuiAdjustableSystem,
		kengine::ImGuiToolSystem,
		kengine::ImGuiEntityEditorSystem,
		kengine::ImGuiEntitySelectorSystem
	>("plugins");

	extern void registerTypes(kengine::EntityManager &);
	registerTypes(em);

	addMenus(em);

	while (em.running)
		em.execute();
	em.send(packets::Terminate{});

	return 0;
}
