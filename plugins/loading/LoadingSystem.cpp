#include <filesystem>
#include <thread>

#include "LoadingSystem.hpp"

#include "EntityManager.hpp"
#include "Export.hpp"

#include "functions/LoadFromJSON.hpp"
#include "json.hpp"

static std::thread * g_loadingThread = nullptr;

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new LoadingSystem(em);
}

LoadingSystem::LoadingSystem(kengine::EntityManager & em) : System(em), _em(em) {
}

void LoadingSystem::handle(kengine::packets::Terminate) {
	if (g_loadingThread == nullptr)
		return;
	g_loadingThread->join();
	delete g_loadingThread;
}

static std::vector<kengine::Entity::ID> g_toEnable;

static void loadEntity(kengine::Entity & e, const putils::json & json, kengine::EntityManager & em, bool active) {
	if (!active) {
		em.setEntityActive(e, false);
		g_toEnable.push_back(e.id);
	}

	for (const auto comp : em.getComponentFunctionMaps()) {
		if (!em.running)
			return;

		const auto loader = comp->getFunction<kengine::functions::LoadFromJSON>();
		if (loader != nullptr)
			loader(json, e);
	}
}

static void loadModels(const std::filesystem::path & dir, kengine::EntityManager & em) {
	namespace fs = std::filesystem;

	putils::vector<putils::string<128>, 64> models;
	static auto loadCurrentModels = [&] {
		for (const auto & file : models) {
			if (!em.running)
				return;
			std::ifstream f(file.c_str());
			em += [&](kengine::Entity & e) { loadEntity(e, putils::json::parse(f), em, true); };
		}
		models.clear();
	};

	for (const auto & entry : fs::recursive_directory_iterator(dir)) {
		if (!em.running)
			return;

		if (entry.path().extension() != ".json")
			continue;

		models.push_back(entry.path().string());
		if (models.full())
			loadCurrentModels();
	}
	loadCurrentModels();
}

static void loadScene(const char * file, kengine::EntityManager & em) {
	std::ifstream f(file);
	static const putils::json fileJSON = putils::json::parse(f);

	for (const auto & json : fileJSON) {
		if (!em.running)
			return;
		em += [&](kengine::Entity & e) { loadEntity(e, json, em, false); };
	}
}

static std::vector<kengine::Entity::ID> g_toRemove;
static void loadTemporaryScene(const char * file, kengine::EntityManager & em) {
	std::ifstream f(file);
	static const putils::json fileJSON = putils::json::parse(f);

	for (const auto & json : fileJSON)
		em += [&](kengine::Entity & e) {
			g_toRemove.push_back(e.id);
			loadEntity(e, json, em, true);
		};
}

static void loadingThread(kengine::EntityManager & em) {
	loadModels("resources/models", em);
	loadScene("resources/scene.json", em);
}

enum LoadingState {
	NotStarted,
	InProgress,
	LoadingDone,
	Complete
};

static std::atomic<LoadingState> loadingState = LoadingState::NotStarted;

static void setupLoading(kengine::EntityManager & em) {
	loadTemporaryScene("resources/loadingScene.json", em);

	g_loadingThread = new std::thread([&] {
		putils::set_thread_name(L"Loading thread");
		loadingThread(em);
		loadingState = LoadingState::LoadingDone;
	});
}

void LoadingSystem::execute() {
	switch (loadingState) {
		case LoadingState::NotStarted: {
			loadingState = LoadingState::InProgress;
			setupLoading(_em);
			break;
		}
		case LoadingState::LoadingDone: {
			for (const auto id : g_toEnable)
				_em.setEntityActive(id, true);
			g_toEnable.clear();

			for (const auto id : g_toRemove)
				_em.removeEntity(id);
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