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

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine meta
#include "kengine/meta/emplace_or_replace.hpp"
#include "kengine/meta/get.hpp"
#include "kengine/meta/has.hpp"

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
		std::thread loading_thread;

		enum class loading_state {
			not_started,
			in_progress,
			done,
			complete
		};

		std::atomic<loading_state> state = loading_state::not_started;

		std::vector<entt::entity> to_remove;

		loading(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<kengine::functions::execute>(putils_forward_to_this(execute));
		}

		~loading() noexcept {
			KENGINE_PROFILING_SCOPE;
			if (loading_thread.joinable())
				loading_thread.join();
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
					state = loading_state::complete;
					if (loading_thread.joinable())
						loading_thread.join();

					r.destroy(to_remove.begin(), to_remove.end());
					to_remove.clear();

					load_scene("resources/scene.json");
					break;
				}
				default: break;
			}
		}

		void setup_loading() noexcept {
			KENGINE_PROFILING_SCOPE;

			load_temporary_scene("resources/loadingScene.json");

			loading_thread = std::thread([this]() noexcept {
				putils::set_thread_name(L"Loading thread");
				run_loading_thread();
				state = loading_state::done;
			});
		}

		nlohmann::json temporary_scene_json;
		void load_temporary_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::ifstream f(file);
			temporary_scene_json = nlohmann::json::parse(f);

			for (const auto & json : temporary_scene_json) {
				const auto e = r.create();
				to_remove.push_back(e);
				kengine::json_helper::load_entity(json, { r, e });
			}
		}

		void run_loading_thread() noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine_log(r, log, "Loading", "Starting");
			load_models("resources/models");
			kengine_log(r, log, "Loading", "Exiting");
		}

		void load_models(const std::filesystem::path & dir) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, "Loading", "Loading models from %s", dir.string().c_str());

			for (const auto & entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (!kengine::is_running(r))
					return;

				if (entry.path().extension() != ".json")
					continue;

				const auto e = r.create();
				std::ifstream f(entry.path());
				kengine::json_helper::load_entity(nlohmann::json::parse(f), { r, e });
			}

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
				r.emplace<data::to_enable>(e);
				kengine::json_helper::load_entity(json, { r, e });
			}

			kengine_log(r, log, "Loading", "Finished loading scene");
		}
	};
}

EXPORT void load_kengine_plugin(entt::registry & r) noexcept {
	const entt::handle e{ r, r.create() };
	e.emplace<systems::loading>(e);
}
