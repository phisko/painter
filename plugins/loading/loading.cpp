#include "Export.hpp"

// stl
#include <filesystem>
#include <thread>
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/string.hpp"
#include "putils/thread_name.hpp"
#include "putils/vector.hpp"

// kengine data
#include "kengine/data/name.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine meta
#include "kengine/meta/has.hpp"
#include "kengine/meta/move.hpp"
#include "kengine/meta/load_from_json.hpp"

// kengine helpers
#include "kengine/helpers/is_running.hpp"
#include "kengine/helpers/json_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace data {
	struct to_enable {};
}

namespace systems {
	struct loading {
		entt::registry & r;

		std::thread * loading_thread = nullptr;

		enum class loading_state {
			not_started,
			in_progress,
			done,
			complete
		};

		std::atomic<loading_state> state = loading_state::not_started;

		std::vector<entt::entity> to_enable;
		std::vector<entt::entity> to_remove;

		loading(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
		}

		~loading() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (loading_thread == nullptr)
				return;
			loading_thread->join();
			delete loading_thread;
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			switch (state) {
				case loading_state::not_started: {
					state = loading_state::in_progress;
					setup_loading();
					break;
				}
				case loading_state::done: {
#if 0
				for (const auto id : to_enable)
					kengine::entities.setActive(id, true);
#endif
					to_enable.clear();

					r.destroy(to_remove.begin(), to_remove.end());
					to_remove.clear();

					state = loading_state::complete;
#if 0
				loading_thread->join();
				delete loading_thread;
				loading_thread = nullptr;
#endif
					break;
				}
				default: break;
			}
		}

		void setup_loading() noexcept {
			KENGINE_PROFILING_SCOPE;

			load_temporary_scene("resources/loadingScene.json");

			// loading_thread = new std::thread([]() noexcept {
			putils::set_thread_name(L"Loading thread");
			run_loading_thread();
			state = loading_state::done;
			// });
		}

		nlohmann::json temporary_scene_json;
		void load_temporary_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::ifstream f(file);
			temporary_scene_json = nlohmann::json::parse(f);

			for (const auto & json : temporary_scene_json) {
				const auto e = r.create();
				to_remove.push_back(e);
				load_entity({ r, e }, json, true);
			}
		}

		void load_entity(entt::handle e, const nlohmann::json & json, bool active) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!active) {
				e.emplace<data::to_enable>();
#if 0
			kengine::entities.setActive(e, false);
			to_enable.push_back(e.id);
#endif
			}

			kengine::json_helper::load_entity(json, e);
		}

		void run_loading_thread() noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine_log(r, log, "Loading", "Starting");
			load_models("resources/models");
			load_scene("resources/scene.json");
			kengine_log(r, log, "Loading", "Exiting");
		}

		void load_models(const std::filesystem::path & dir) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, "Loading", "Loading models from %s", dir.string().c_str());

			namespace fs = std::filesystem;

			putils::vector<putils::string<128>, 64> models;

			auto load_current_models = [&]() noexcept {
				for (const auto & file : models) {
					if (!kengine::is_running(r))
						return;
					const auto e = r.create();
					std::ifstream f(file.c_str());
					load_entity({ r, e }, nlohmann::json::parse(f), true);
				}
				models.clear();
			};

			for (const auto & entry : fs::recursive_directory_iterator(dir)) {
				if (!kengine::is_running(r))
					return;

				if (entry.path().extension() != ".json")
					continue;

				models.push_back(entry.path().string());
				if (models.full())
					load_current_models();
			}
			load_current_models();

			kengine_log(r, log, "Loading", "Finished loading models");
		}

		nlohmann::json scene_json;
		void load_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, "Loading", "Loading scene from %s", file);

			std::ifstream f(file);
			scene_json = nlohmann::json::parse(f);

			for (const auto & json : scene_json) {
				if (!kengine::is_running(r))
					return;
				const auto e = r.create();
				load_entity({ r, e }, json, false);
			}

			kengine_log(r, log, "Loading", "Finished loading scene");
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::loading>(e);
}
