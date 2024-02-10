#pragma once
#include "../../core/bedstation.h"

namespace llpp::bots::render
{
	class RenderStation final : public core::BedStation
	{
	public:
		RenderStation(std::string name, std::chrono::seconds load_for);

		core::StationResult complete() override;

	private:
		std::chrono::seconds render_duration;
		inline static asa::structures::SimpleBed gateway_bed{
			"RENDER::GATEWAY"
		};
	};
};