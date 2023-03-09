#pragma once

namespace camera {
	//! putils reflect all
	//! class_name: camera_config
	//! metadata: [("config", true)]
	struct config {
		float mouse_sensitivity = .005f;
		float movement_speed = 10.f;
		float movement_speed_modifier = 2.f;
		float zoom_speed = .1f;
	};
}

#include "config.rpp"