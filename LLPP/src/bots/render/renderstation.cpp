#include "renderstation.h"
#include "../../common/util.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/tribemanager.h>
#include "../../core/discord.h"

namespace llpp::bots::render
{

	RenderStation::RenderStation(
		std::string t_name, std::chrono::seconds t_load_for)
		: render_duration(t_load_for),
		  BaseStation(t_name, std::chrono::minutes(5)),
		  src_bed(asa::structures::SimpleBed(t_name)){};

	llpp::core::StationResult RenderStation::complete()
	{
		auto start = std::chrono::system_clock::now();

        asa::entities::local_player->fast_travel_to(src_bed, AccessFlags_Default,
                                                    TravelFlags_NoTravelAnimation);

        asa::interfaces::tribe_manager->update_tribelogs(core::discord::handle_tribelogs,
                                                         std::chrono::seconds(3));
		std::this_thread::sleep_for(render_duration);

        asa::entities::local_player->fast_travel_to(gateway_bed, AccessFlags_Default,
                                                    TravelFlags_NoTravelAnimation);

        asa::interfaces::tribe_manager->update_tribelogs(core::discord::handle_tribelogs,
                                                         std::chrono::seconds(0));
		set_completed();

		auto duration = util::get_elapsed<std::chrono::seconds>(start);
		return core::StationResult(this, true, duration, {});
	}
}
