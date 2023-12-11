#include "embeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include "../../core/stationresult.h"
#include <asapp/game/window.h>
#include <format>

void llpp::bots::paste::SendSuccessEmbed(const core::StationResult& data)
{
	auto nextCompletion = std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::now() +
		data.station->GetCompletionInterval());

	dpp::embed embed = dpp::embed();
	embed.set_color(dpp::colors::gray)
		.set_title(std::format(
			"Paste Station '{}' has been completed.", data.station->GetName()))
		.set_description(
			std::format("The station was completed successfully {} times!",
				data.station->GetTimesCompleted()))
		.set_thumbnail("https://static.wikia.nocookie.net/"
					   "arksurvivalevolved_gamepedia/images/0/03/"
					   "Cementing_Paste.png/revision/latest?cb=20180801020251")
		.add_field("Time taken:",
			std::format("{} seconds", data.timeTaken.count()), true)
		.add_field("Paste obtained:",
			std::to_string(data.obtainedItems.at("Achatina Paste")), true)
		.add_field(
			"Next completion:", std::format("<t:{}:R>", nextCompletion), true);


	dpp::message msg = dpp::message(
		dpp::snowflake(llpp::core::discord::infoChannelID), embed);

	llpp::core::discord::bot->message_create(msg);
}

void llpp::bots::paste::SendAchatinaNotAccessible(
	const std::string& stationName, const std::string& achatinaName)
{
	dpp::embed embed = dpp::embed();
	embed.set_color(0xf6c330)
		.set_title(std::format("Problem with Achatina at '{}'!", stationName))
		.set_description(std::format(
			"An achatina may be inaccessible, manual check required.",
			achatinaName))
		.set_thumbnail(
			"https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/"
			"images/f/f6/Radiation.png/revision/latest?cb=20171216191751")
		.add_field("Station:", stationName, true)
		.add_field("Achatina: ", achatinaName, true)
		.set_image("attachment://image.png");

	auto fileData = util::MatToStringBuffer(asa::window::Screenshot());
	dpp::message message =
		dpp::message(dpp::snowflake(llpp::core::discord::infoChannelID),
			"<@&1181159721433051136>")
			.set_allowed_mentions(false, true, false, false, {}, {});
	message.add_file("image.png", fileData, "image/png ").add_embed(embed);


	llpp::core::discord::bot->message_create(message);
}
