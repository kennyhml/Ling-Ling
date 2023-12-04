#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::render
{
	class RenderStation final : public core::BaseStation
	{
	public:
		RenderStation(std::string name, std::chrono::seconds loadFor);

		core::StationResult Complete() override;

	private:
		std::chrono::seconds renderDuration;
		asa::structures::SimpleBed srcBed;
		inline static asa::structures::SimpleBed gatewayBed{
			"RENDER::GATEWAY"
		};
	};
};