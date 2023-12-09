#pragma once
#include "../../core/stationresult.h"
#include "cratemanager.h"
#include <asapp/structures/cavelootcrate.h>
#include <format>

namespace llpp::bots::drops
{
	const std::string WHITE_CRATE_THUMBNAIL =
		"https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
		"8/8c/White_Crate.png/revision/latest?cb=20190116151008";

	const std::string BLUE_CRATE_THUMBNAIL =
		"https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
		"9/9c/Blue_Crate.png/revision/latest/scale-to-width-down/"
		"109?cb=20190116151056";

	const std::string YELLOW_CRATE_THUMBNAIL =
		"https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
		"6/63/Yellow_Crate.png/revision/latest?cb=20190116151124";

	const std::string RED_CRATE_THUMBNAIL =
		"https://static.wikia.nocookie.net/arksurvivalevolved_gamepedia/images/"
		"c/c5/Red_Crate.png/revision/latest?cb=20190116151200";

	void SendSuccessEmbed(const core::StationResult&, cv::Mat lootScreenshot,
		asa::structures::CaveLootCrate::Quality, int timesLooted);

	void SendSummaryEmbed(const std::string& name,
		std::chrono::seconds timeTaken,
		std::vector<CrateManager::CrateGroupStatistics> stats);
}