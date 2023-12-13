#pragma once
#include "../../core/istationmanager.h"
#include "../render/renderstation.h"
#include "pastestation.h"
#include <memory>

namespace llpp::bots::paste
{
	class PasteManager final : public core::IStationManager
	{
	public:
		PasteManager(std::string t_prefix, int t_num_stations,
			std::chrono::minutes t_interval);

		bool run() override;
		bool is_ready_to_run() const override;
		std::chrono::minutes get_time_left_until_ready() const override;

		const PasteStation* peek_station(int index) const;

	private:
		std::vector<std::unique_ptr<PasteStation>> stations;
		render::RenderStation render_station{ "PASTE::RENDER::SRC",
			std::chrono::seconds(10) };
	};
}