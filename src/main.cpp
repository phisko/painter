#include "go_to_bin_dir.hpp"

#include "systems/LuaSystem.hpp"
#include "systems/PySystem.hpp"
#include "systems/BehaviorSystem.hpp"
#include "systems/OnClickSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/ImGuiAdjustableSystem.hpp"
#include "systems/ImGuiEntityEditorSystem.hpp"
#include "systems/ImGuiEntitySelectorSystem.hpp"
#include "systems/polyvox/PolyVoxSystem.hpp"
#include "systems/polyvox/MagicaVoxelSystem.hpp"
#include "systems/assimp/AssimpSystem.hpp"
#include "systems/opengl/OpenGLSystem.hpp"
#include "systems/opengl_sprites/OpenGLSpritesSystem.hpp"
#include "systems/bullet/BulletSystem.hpp"

#include "components/ImGuiComponent.hpp"
#include "imgui.h"

#include "packets/GBuffer.hpp"
#include "packets/Terminate.hpp"

#include "types/registerTypes.hpp"

auto TimeControls(kengine::EntityManager & em) {
	static bool display = true;
	em.send(kengine::packets::AddImGuiTool{ .name = "Time controller", .enabled = display });

	return [&em](kengine::Entity & e) {
		e += kengine::ImGuiComponent([&em] {
			if (!display)
				return;

			if (ImGui::Begin("Time controls", &display)) {
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
	em += [&em](kengine::Entity & e) {
		e += kengine::ImGuiComponent([&em] {
			if (ImGui::BeginMainMenuBar()) {

				if (ImGui::BeginMenu("Save")) {

					if (ImGui::MenuItem("Save"))
						em.save();
					if (ImGui::MenuItem("Load"))
						em.runAfterSystem([&em] { em.load(); });

					ImGui::EndMenu();
				}

			ImGui::EndMainMenuBar(); }
		});
	};

	em += TimeControls(em);
}

int main(int, char **av) {
	putils::goToBinDir(av[0]);

#if defined(_WIN32) && defined(NDEBUG)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	kengine::EntityManager em(std::thread::hardware_concurrency());

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
		kengine::ImGuiEntityEditorSystem,
		kengine::ImGuiEntitySelectorSystem
	>("plugins");

	registerTypes(em);

	addMenus(em);
	em.onLoad([&em](const char *) { addMenus(em); });

	while (em.running) {
		em.execute();
		em.loadSystems("plugins");
	}
	em.send(packets::Terminate{});

	return 0;
}
