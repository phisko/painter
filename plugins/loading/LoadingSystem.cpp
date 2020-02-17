#include <filesystem>
#include <thread>

#include "EntityManager.hpp"
#include "Export.hpp"

#include "functions/Execute.hpp"
#include "functions/OnTerminate.hpp"

#include "meta/LoadFromJSON.hpp"
#include "json.hpp"

#include "helpers/PluginHelper.hpp"

static kengine::EntityManager * g_em;

static std::thread * g_loadingThread = nullptr;

// declarations
static void execute(float deltaTime);
static void onTerminate();
//
EXPORT void loadKenginePlugin(kengine::EntityManager & em) {
	kengine::PluginHelper::initPlugin(em);

	g_em = &em;

	em += [](kengine::Entity & e) {
		e += kengine::functions::Execute{ execute };
		e += kengine::functions::OnTerminate{ onTerminate };
	};
}

static void onTerminate() {
	if (g_loadingThread == nullptr)
		return;
	g_loadingThread->join();
	delete g_loadingThread;
}

enum LoadingState {
	NotStarted,
	InProgress,
	LoadingDone,
	Complete
};
static std::atomic<LoadingState> loadingState = LoadingState::NotStarted;

static std::vector<kengine::Entity::ID> g_toEnable;
static std::vector<kengine::Entity::ID> g_toRemove;

// declarations
static void setupLoading(kengine::EntityManager & em);
//
static void execute(float deltaTime) {
	switch (loadingState) {
		case LoadingState::NotStarted: {
			loadingState = LoadingState::InProgress;
			setupLoading(*g_em);
			break;
		}
		case LoadingState::LoadingDone: {
			for (const auto id : g_toEnable)
				g_em->setEntityActive(id, true);
			g_toEnable.clear();

			for (const auto id : g_toRemove)
				g_em->removeEntity(id);
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

// declarations
static void loadTemporaryScene(const char * file, kengine::EntityManager & em);
static void loadingThread(kengine::EntityManager & em);
//
static void setupLoading(kengine::EntityManager & em) {
	loadTemporaryScene("resources/loadingScene.json", em);

	g_loadingThread = new std::thread([&] {
		putils::set_thread_name(L"Loading thread");
		loadingThread(em);
		loadingState = LoadingState::LoadingDone;
	});
}

// declarations
static void loadEntity(kengine::Entity & e, const putils::json & json, kengine::EntityManager & em, bool active);
//
static void loadTemporaryScene(const char * file, kengine::EntityManager & em) {
	std::ifstream f(file);
	static const putils::json fileJSON = putils::json::parse(f);

	for (const auto & json : fileJSON)
		em += [&](kengine::Entity & e) {
			g_toRemove.push_back(e.id);
			loadEntity(e, json, em, true);
		};
}

// declarations
static void loadModels(const std::filesystem::path & dir, kengine::EntityManager & em);
static void loadScene(const char * file, kengine::EntityManager & em);
//
static void loadingThread(kengine::EntityManager & em) {
	loadModels("resources/models", em);
	loadScene("resources/scene.json", em);
}

static void loadModels(const std::filesystem::path & dir, kengine::EntityManager & em) {
	namespace fs = std::filesystem;

	putils::vector<putils::string<128>, 64> models;
	static auto loadCurrentModels = [&] {
		for (const auto & file : models) {
			if (!em.running)
				return;
			em += [&](kengine::Entity & e) {
				std::ifstream f(file.c_str());
				loadEntity(e, putils::json::parse(f), em, true);
			};
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

#include "data/NameComponent.hpp"
static void loadEntity(kengine::Entity & e, const putils::json & json, kengine::EntityManager & em, bool active) {
	if (!active) {
		em.setEntityActive(e, false);
		g_toEnable.push_back(e.id);
	}

	for (const auto & [_, loader, name] : em.getEntities<kengine::meta::LoadFromJSON, kengine::NameComponent>()) {
		if (!em.running)
			return;
#ifndef KENGINE_NDEBUG
		if (json.find(name.name.c_str()) == json.end()) // not necessary, only for debug
			continue; 
#endif
		loader(json, e);
	}
}
