#pragma once

namespace character_animation {
	//! putils reflect all
	//! class_name: character_animation_config
	//! metadata: [("config", true)]
	struct config {
		float wobble_speed = 1.f;
		float max_roll = 1.f;
	};
}

#include "config.rpp"