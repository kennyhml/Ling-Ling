#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::render
{
	class RenderStation final : public core::LLPPBaseStation
	{
	public:
		RenderStation(std::string name, std::chrono::seconds loadFor)
			: renderDuration(loadFor),
			  LLPPBaseStation(name, std::chrono::minutes(5)),
			  srcBed(asa::structures::SimpleBed(name)){};

		virtual const core::StationResult Complete() override;

	private:
		std::chrono::seconds renderDuration;
		asa::structures::SimpleBed srcBed;
		inline static asa::structures::SimpleBed gatewayBed{
			"RENDER::GATEWAY"
		};
	};
};