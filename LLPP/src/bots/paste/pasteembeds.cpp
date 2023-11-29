#include "pasteembeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/stationresult.h"
#include "../../core/webhook.h"
#include <asapp/game/window.h>
#include <format>

void llpp::bots::paste::SendSuccessEmbed(const core::StationResult& data)

{
	dpp::embed embed = dpp::embed();
	embed.set_color(dpp::colors::gray)
		.set_title(std::format(
			"Paste Station '{}' has been completed.", data.station->GetName()))
		.set_description(
			std::format("The station was completed successfully {} times!",
				data.completions))
		.set_thumbnail("https://static.wikia.nocookie.net/"
					   "arksurvivalevolved_gamepedia/images/0/03/"
					   "Cementing_Paste.png/revision/latest?cb=20180801020251")
		.add_field("Time taken:",
			std::format("{} seconds", data.timeTaken.count()), true)
		.add_field("Paste obtained:", "300", true)
		.add_field("Next completion:",
			std::format(
				"{} minutes", data.station->GetCompletionInterval().count()),
			true);

	core::discord::Send(embed, core::discord::webhook);
}

void llpp::bots::paste::SendAchatinaNotAccessible(
	const std::string& stationName, asa::entities::DinoEnt* achatina)
{
	dpp::embed embed = dpp::embed();
	embed.set_color(dpp::colors::brown_bear)
		.set_title(std::format("Problem with Achatina at '{}'", stationName))
		.set_description(std::format(
			"Failed to access '{}', please check.", achatina->GetName()))
		.set_thumbnail(
			"https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/"
			"images/2/24/Crafting_Light.png/revision/latest?cb=20181217123945")
		.add_field("Station:", stationName, true)
		.add_field("Achatina: ", achatina->GetName(), true)
		.set_image("attachment://image.png");

	auto fileData = util::MatToStringBuffer(asa::window::Screenshot());
	dpp::message message = dpp::message();
	message.add_file("image.png", fileData, "image/png ").add_embed(embed);

	core::discord::Send(message, core::discord::webhook);
}
