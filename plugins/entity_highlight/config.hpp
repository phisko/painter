#pragma once

// putils
#include "putils/color.hpp"

namespace entity_highlight {
	//! putils reflect all
	//! class_name: entity_highlight_config
	//! metadata: [("config", true)]
	struct config {
		putils::normalized_color selected_color;
		float selected_intensity = 2.f;

		putils::normalized_color hovered_color;
		float hovered_intensity = 1.f;
	};
}

#include "config.rpp"