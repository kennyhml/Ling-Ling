#include "embeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/webhook.h"
#include <format>

void llpp::bots::drops::SendSuccessEmbed(const core::StationResult& data,
	cv::Mat lootScreenshot, asa::structures::CaveLootCrate::Quality quality,
	int timesLooted)
{
	using Quality = asa::structures::CaveLootCrate::Quality;

	auto color = dpp::colors::white;
	std::string thumbnailUrl = WHITE_CRATE_THUMBNAIL;
	std::string dropQuality = "Undetermined";
	switch (quality) {
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

	auto nextCompletion = std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::now() +
		data.station->GetCompletionInterval());

	dpp::embed embed = dpp::embed();
	embed.set_color(color)
		.set_title(
			std::format("Crate '{}' has been looted!", data.station->GetName()))
		.set_description(std::format("This crate has been looted {}/{} times!",
			timesLooted, data.station->GetTimesCompleted()))
		.set_thumbnail(thumbnailUrl)
		.add_field("Time taken:",
			std::format("{} seconds", data.timeTaken.count()), true)
		.add_field("Crate Quality:", dropQuality, true)
		.add_field(
			"Next completion:", std::format("<t:{}:R>", nextCompletion), true)
		.set_image("attachment://image.png");

	auto fileData = util::MatToStringBuffer(lootScreenshot);
	dpp::message message = dpp::message();
	message.add_file("image.png", fileData, "image/png ").add_embed(embed);
	core::discord::Send(message, core::discord::dropWebhook);
}

void llpp::bots::drops::SendSummaryEmbed(const std::string& name,
	std::chrono::seconds timeTaken,
	std::vector<CrateManager::CrateGroupStatistics> stats, int dropoffFilled,
	std::chrono::system_clock::time_point nextCompletion)
{
	dpp::embed embed = dpp::embed();
	embed.set_color(dpp::colors::yellow)
		.set_title(std::format("Crate Manager '{}' has been completed!", name))
		.set_description("Here is a summary of this crate managers data:")
		.set_thumbnail(YELLOW_CRATE_THUMBNAIL)
		.add_field(
			"Time taken:", std::format("{} seconds", timeTaken.count()), true)
		.add_field("Vault filled:", std::format("{}%", dropoffFilled), true)
		.add_field("Next completion:",
			std::format("<t:{}:R>",
				std::chrono::system_clock::to_time_t(nextCompletion)),
			true)
		.set_image("attachment://image.png");

	for (int i = 0; i < stats.size(); i++) {
		auto respawnTime = std::chrono::duration_cast<std::chrono::minutes>(
			stats[i].GetAverageRespawnTime());

		embed.add_field("",
			std::format(">>> **__Group {}:__** \nAvg. respawn: {} "
						"\nTimes looted: {}",
				i + 1, respawnTime, stats[i].GetTimesLooted()),
			true);
	}

	dpp::message message = dpp::message().add_embed(embed);
	core::discord::Send(message, core::discord::webhook);
}