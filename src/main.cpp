#include "go_to_bin_dir.hpp"
 
 #include "systems/LuaSystem.hpp"
 #include "systems/PySystem.hpp"
 #include "systems/BehaviorSystem.hpp"
 #include "systems/OnClickSystem.hpp"
 #include "systems/PhysicsSystem.hpp"
 
 #include "systems/ImGuiAdjustableSystem.hpp"
 #include "systems/ImGuiEntityEditorSystem.hpp"
 #include "systems/ImGuiEntitySelectorSystem.hpp"
 #include "systems/ImGuiToolSystem.hpp"
 
 #include "systems/ogre/OgreSystem.hpp"
 
 #include "components/ImGuiComponent.hpp"
 #include "imgui.h"
 
 #include "packets/Terminate.hpp"
 
 auto TimeControls(kengine::EntityManager & em) {
 	return [&em](kengine::Entity & e) {
 		auto & tool = e.attach<kengine::ImGuiToolComponent>();
 		tool.enabled = false;
 		tool.name = "Time controller";
 
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
 
 #if defined(_WIN32) && defined(KENGINE_NDEBUG)
 	ShowWindow(GetConsoleWindow(), SW_HIDE);
 #endif
 
 	kengine::EntityManager em(std::thread::hardware_concurrency());
 
 	em.loadSystems<
 		kengine::LuaSystem, kengine::PySystem,
 		kengine::BehaviorSystem,
 		kengine::OnClickSystem,
 
 		kengine::OgreSystem,
 
 		kengine::ImGuiAdjustableSystem,
 		kengine::ImGuiEntityEditorSystem,
 		kengine::ImGuiEntitySelectorSystem,
 		kengine::ImGuiToolSystem
 	>("plugins");
 
 	extern void registerTypes(kengine::EntityManager &);
 	registerTypes(em);
 
 	addMenus(em);
 	em.onLoad([&em](const char *) { addMenus(em); });
 
 #if defined(_WIN32) && !defined(KENGINE_NDEBUG)
 	ShowWindow(GetConsoleWindow(), SW_SHOW);
 #endif
 
 	while (em.running) {
 		em.execute();
 		em.loadSystems("plugins");
 	}
 	em.send(packets::Terminate{});
 
 	return 0;
 }