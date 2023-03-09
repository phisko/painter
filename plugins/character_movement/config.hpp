#pragma once

// putils
#include "putils/angle.hpp"

namespace character_movement {
	//! putils reflect all
	//! class_name: character_movement_config
	//! metadata: [("config", true)]
	struct config {
		float facing_strictness = 0.05f;
		float turn_speed = putils::pi;
	};
}

#include "config.rpp"