#include "embeds.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "../../core/discord.h"
#include <format>


namespace llpp::bots::drops
{
	void llpp::bots::drops::send_success_embed(const core::StationResult& data,
		cv::Mat loot, asa::structures::CaveLootCrate::Quality quality,
		int times_looted)
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

		auto next_completion = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now() +
			data.station->get_completion_interval());

		dpp::embed embed = dpp::embed();
		embed.set_color(color)
			.set_title(std::format(
				"Crate '{}' has been looted!", data.station->get_name()))
			.set_description(
				std::format("This crate has been looted {}/{} times!",
					times_looted, data.station->get_times_completed()))
			.set_thumbnail(thumbnailUrl)
			.add_field("Time taken:",
				std::format("{} seconds", data.time_taken.count()), true)
			.add_field("Crate Quality:", dropQuality, true)
			.add_field("Next completion:",
				std::format("<t:{}:R>", next_completion), true)
			.set_image("attachment://image.png");

		auto fileData = util::mat_to_strbuffer(loot);
		dpp::message message = dpp::message(
			core::discord::infoChannelID, embed);
		message.add_file("image.png", fileData, "image/png ");

		core::discord::bot->message_create(message);
	}

	void send_summary_embed(const std::string& name,
		std::chrono::seconds time_taken,
		std::vector<CrateManager::CrateGroupStatistics> stats, int vault_slots,
		std::chrono::system_clock::time_point next_completion)
	{
		dpp::embed embed = dpp::embed();
		embed.set_color(dpp::colors::yellow)
			.set_title(
				std::format("Crate Manager '{}' has been completed!", name))
			.set_description("Here is a summary of this crate managers data:")
			.set_thumbnail(YELLOW_CRATE_THUMBNAIL)
			.add_field("Time taken:",
				std::format("{} seconds", time_taken.count()), true)
			.add_field("Vault filled:", std::format("{}%", vault_slots), true)
			.add_field("Next completion:",
				std::format("<t:{}:R>",
					std::chrono::system_clock::to_time_t(next_completion)),
				true)
			.set_image("attachment://image.png");

		for (int i = 0; i < stats.size(); i++) {
			auto respawnTime = std::chrono::duration_cast<std::chrono::minutes>(
				stats[i].get_avg_respawn());

			embed.add_field("",
				std::format(">>> **__Group {}:__** \nRespawn: {} "
							"\nTimes looted: {}",
					i + 1, respawnTime, stats[i].get_times_looted()),
				true);
		}

		dpp::message message = dpp::message(
			core::discord::infoChannelID, embed);
		core::discord::bot->message_create(message);
	}

}
