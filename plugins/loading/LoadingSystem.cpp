#include "kengine.hpp"
#include "Export.hpp"

// stl
#include <filesystem>
#include <thread>
#include <fstream>

// nlohmann
#include <nlohmann/json.hpp>

// putils
#include "vector.hpp"
#include "string.hpp"
#include "thread_name.hpp"

// kengine data
#include "data/NameComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"
#include "functions/OnTerminate.hpp"

// kengine meta
#include "meta/LoadFromJSON.hpp"

// kengine helpers
#include "helpers/pluginHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

struct LoadingSystem {
	static void init() noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine::entities += [](kengine::Entity & e) noexcept {
			e += kengine::functions::Execute{ execute };
			e += kengine::functions::OnTerminate{ onTerminate };
		};
	}

	static void onTerminate() noexcept {
		KENGINE_PROFILING_SCOPE;

		if (_loadingThread == nullptr)
			return;
		_loadingThread->join();
		delete _loadingThread;
	}

	static void execute(float deltaTime) noexcept {
		KENGINE_PROFILING_SCOPE;

		switch (_loadingState) {
			case LoadingState::NotStarted: {
				_loadingState = LoadingState::InProgress;
				setupLoading();
				break;
			}
			case LoadingState::LoadingDone: {
				for (const auto id : _toEnable)
					kengine::entities.setActive(id, true);
				_toEnable.clear();

				for (const auto id : _toRemove)
					kengine::entities.remove(id);
				_toRemove.clear();

				_loadingState = LoadingState::Complete;
				_loadingThread->join();
				delete _loadingThread;
				_loadingThread = nullptr;
				break;
			}
			default: break;
		}
	}

	static void setupLoading() noexcept {
		KENGINE_PROFILING_SCOPE;

		loadTemporaryScene("resources/loadingScene.json");

		_loadingThread = new std::thread([]() noexcept {
			putils::set_thread_name(L"Loading thread");
			loadingThread();
			_loadingState = LoadingState::LoadingDone;
		});
	}

	static void loadTemporaryScene(const char * file) noexcept {
		KENGINE_PROFILING_SCOPE;

		std::ifstream f(file);
		static const auto fileJSON = nlohmann::json::parse(f);

		for (const auto & json : fileJSON)
			kengine::entities += [&](kengine::Entity & e) noexcept {
				_toRemove.push_back(e.id);
				loadEntity(e, json, true);
			};
	}

	static void loadEntity(kengine::Entity & e, const nlohmann::json & json, bool active) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (!active) {
			kengine::entities.setActive(e, false);
			_toEnable.push_back(e.id);
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
		KENGINE_PROFILING_SCOPE;

		kengine_log(Log, "Loading", "Starting");
		loadModels("resources/models");
		loadScene("resources/scene.json");
		kengine_log(Log, "Loading", "Exiting");
	}

	static void loadModels(const std::filesystem::path & dir) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(Log, "Loading", "Loading models from %s", dir.c_str());

		namespace fs = std::filesystem;

		putils::vector<putils::string<128>, 64> models;
		static auto loadCurrentModels = [&]() noexcept {
			for (const auto & file : models) {
				if (!kengine::isRunning())
					return;
				kengine::entities += [&](kengine::Entity & e) noexcept {
					std::ifstream f(file.c_str());
					loadEntity(e, nlohmann::json::parse(f), true);
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

		kengine_log(Log, "Loading", "Finished loading models");
	}

	static void loadScene(const char * file) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(Log, "Loading", "Loading scene from %s", file);

		std::ifstream f(file);
		static const auto fileJSON = nlohmann::json::parse(f);

		for (const auto & json : fileJSON) {
			if (!kengine::isRunning())
				return;
			kengine::entities += [&](kengine::Entity & e) noexcept { loadEntity(e, json, false); };
		}

		kengine_log(Log, "Loading", "Finished loading scene");
	}

	static inline std::thread * _loadingThread = nullptr;

	enum LoadingState {
		NotStarted,
		InProgress,
		LoadingDone,
		Complete
	};
	static inline std::atomic<LoadingState> _loadingState = LoadingState::NotStarted;

	static inline std::vector<kengine::EntityID> _toEnable;
	static inline std::vector<kengine::EntityID> _toRemove;
};

EXPORT void loadKenginePlugin(void * state) noexcept {
	kengine::pluginHelper::initPlugin(state);
	LoadingSystem::init();
}
