#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::render
{
	class RenderStation final : public core::BaseStation
	{
	public:
		RenderStation(std::string name, std::chrono::seconds load_for);

		core::StationResult complete() override;

	private:
		std::chrono::seconds render_duration;
		asa::structures::SimpleBed src_bed;
		inline static asa::structures::SimpleBed gateway_bed{
			"RENDER::GATEWAY"
		};
	};
};