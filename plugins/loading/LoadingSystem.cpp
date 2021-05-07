#include <filesystem>
#include <thread>
#include <fstream>

#include "kengine.hpp"
#include "Export.hpp"

#include "data/NameComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnTerminate.hpp"

#include "meta/LoadFromJSON.hpp"
#include "json.hpp"

#include "helpers/pluginHelper.hpp"
#include "helpers/logHelper.hpp"

#include "vector.hpp"
#include "string.hpp"
#include "thread_name.hpp"

static std::thread * g_loadingThread = nullptr;

enum LoadingState {
	NotStarted,
	InProgress,
	LoadingDone,
	Complete
};
static std::atomic<LoadingState> loadingState = LoadingState::NotStarted;

static std::vector<kengine::EntityID> g_toEnable;
static std::vector<kengine::EntityID> g_toRemove;


EXPORT void loadKenginePlugin(void * state) noexcept {
	struct impl {
		static void init() noexcept {
			kengine::entities += [](kengine::Entity & e) noexcept {
				e += kengine::functions::Execute{ execute };
				e += kengine::functions::OnTerminate{ onTerminate };
			};
		}

		static void onTerminate() noexcept {
			if (g_loadingThread == nullptr)
				return;
			g_loadingThread->join();
			delete g_loadingThread;
		}

		static void execute(float deltaTime) noexcept {
			switch (loadingState) {
			case LoadingState::NotStarted: {
				loadingState = LoadingState::InProgress;
				setupLoading();
				break;
			}
			case LoadingState::LoadingDone: {
				for (const auto id : g_toEnable)
					kengine::entities.setActive(id, true);
				g_toEnable.clear();

				for (const auto id : g_toRemove)
					kengine::entities.remove(id);
				g_toRemove.clear();

				loadingState = LoadingState::Complete;
				g_loadingThread->join();
				delete g_loadingThread;
				g_loadingThread = nullptr;
				break;
			}
			default: break;
			}
		}

		static void setupLoading() noexcept {
			loadTemporaryScene("resources/loadingScene.json");

			g_loadingThread = new std::thread([]() noexcept {
				putils::set_thread_name(L"Loading thread");
				loadingThread();
				loadingState = LoadingState::LoadingDone;
				});
		}

		static void loadTemporaryScene(const char * file) noexcept {
			std::ifstream f(file);
			static const putils::json fileJSON = putils::json::parse(f);

			for (const auto & json : fileJSON)
				kengine::entities += [&](kengine::Entity & e) noexcept {
				g_toRemove.push_back(e.id);
				loadEntity(e, json, true);
			};
		}

		static void loadEntity(kengine::Entity & e, const putils::json & json, bool active) noexcept {
			if (!active) {
				kengine::entities.setActive(e, false);
				g_toEnable.push_back(e.id);
			}

			for (const auto & [_, loader, name] : kengine::entities.with<kengine::meta::LoadFromJSON, kengine::NameComponent>()) {
				if (!kengine::isRunning())
					return;
				if (json.find(name.name.c_str()) == json.end()) // not necessary, only for debug
					continue;
				loader(json, e);
			}
		}

		static void loadingThread() noexcept {
			loadModels("resources/models");
			loadScene("resources/scene.json");
		}

		static void loadModels(const std::filesystem::path & dir) noexcept {
			kengine_logf(Log, "Loading", "Loading models from %s", dir.c_str());

			namespace fs = std::filesystem;

			putils::vector<putils::string<128>, 64> models;
			static auto loadCurrentModels = [&]() noexcept {
				for (const auto & file : models) {
					if (!kengine::isRunning())
						return;
					kengine::entities += [&](kengine::Entity & e) noexcept {
						std::ifstream f(file.c_str());
						loadEntity(e, putils::json::parse(f), true);
					};
				}
				models.clear();
			};

			for (const auto & entry : fs::recursive_directory_iterator(dir)) {
				if (!kengine::isRunning())
					return;

				if (entry.path().extension() != ".json")
					continue;

				models.push_back(entry.path().string());
				if (models.full())
					loadCurrentModels();
			}
			loadCurrentModels();
		}

		static void loadScene(const char * file) noexcept {
			kengine_logf(Log, "Loading", "Loading scene from %s", file);

			std::ifstream f(file);
			static const putils::json fileJSON = putils::json::parse(f);

			for (const auto & json : fileJSON) {
				if (!kengine::isRunning())
					return;
				kengine::entities += [&](kengine::Entity & e) noexcept { loadEntity(e, json, false); };
			}
		}
	};

	kengine::pluginHelper::initPlugin(state);
	impl::init();
}
