#pragma once

namespace physics_test {
	//! putils reflect all
	//! class_name: physics_test_config
	//! metadata: [("config", true)]
	struct config {
		float particle_duration = 5.f;
		int spawn_count = 1;
	};
}

#include "config.rpp"