#include "crateembeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/webhook.h"
#include <format>

void llpp::bots::drops::SendSuccessEmbed(const core::StationResult& data,
	cv::Mat lootScreenshot,
	asa::structures::CaveLootCrate::Quality determinedQuality)
{
	using Quality = asa::structures::CaveLootCrate::Quality;

	auto color = dpp::colors::white;
	std::string thumbnailUrl = WHITE_CRATE_THUMBNAIL;
	std::string dropQuality = "Undetermined";
	switch (determinedQuality) {
	case Quality::RED:
		color = dpp::colors::red_blood;
		thumbnailUrl = RED_CRATE_THUMBNAIL;
		dropQuality = "RED";
		break;
	case Quality::YELLOW:
		color = dpp::colors::yellow;
		thumbnailUrl = YELLOW_CRATE_THUMBNAIL;
		dropQuality = "YELLOW";
		break;
	case Quality::BLUE:
		color = dpp::colors::blue;
		thumbnailUrl = BLUE_CRATE_THUMBNAIL;
		dropQuality = "BLUE";
		break;
	default:
		break;
	}

	dpp::embed embed = dpp::embed();
	embed.set_color(color)
		.set_title(
			std::format("Crate '{}' has been looted!", data.station->GetName()))
		.set_description(std::format(
			"This crate has been looted {} times!", data.completions))
		.set_thumbnail(thumbnailUrl)
		.add_field("Time taken:",
			std::format("{} seconds", data.timeTaken.count()), true)
		.add_field("Crate Quality:", dropQuality, true)
		.add_field("Next completion:",
			std::format(
				"{} minutes", data.station->GetCompletionInterval().count()),
			true)
		.set_image("attachment://image.png");

	auto fileData = util::MatToStringBuffer(lootScreenshot);
	dpp::message message = dpp::message();
	message.add_file("image.png", fileData, "image/png ").add_embed(embed);
	core::discord::Send(message, core::discord::dropWebhook);
}