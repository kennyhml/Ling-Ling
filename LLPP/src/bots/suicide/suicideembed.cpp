#include "suicideembed.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include <format>

void llpp::bots::suicide::SendSuicided(const core::StationResult& data,
	const std::string& atBed, const std::string& respawningAt)
{

	dpp::embed embed = dpp::embed();
	embed.set_color(dpp::colors::light_blue)
		.set_title(std::format("Successfully suicided at '{}'.", atBed))
		.set_description(std::format(
			"Suicided a total of {} times!", data.station->GetTimesCompleted()))
		.set_thumbnail("https://static.wikia.nocookie.net/"
					   "arksurvivalevolved_gamepedia/images/5/55/"
					   "Specimen_Implant.png/revision/latest?cb=20180731184153")
		.add_field("Time taken:",
			std::format("{} seconds", data.timeTaken.count()), true)
		.add_field("Respawning at:", respawningAt, true);

	dpp::message message = dpp::message(
		dpp::snowflake(llpp::core::discord::infoChannelID), embed);
	core::discord::bot->message_create(message);
}
