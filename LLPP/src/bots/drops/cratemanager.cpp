#include "cratemanager.h"
#include "../../common/util.h"
#include "../../core/discord.h"
#include "embeds.h"
#include <asapp/entities/localplayer.h>
using namespace llpp::bots::drops;

CrateManager::CrateManager(std::string prefix,
	std::vector<std::vector<QualityFlags>> groupedCrates,
	std::chrono::minutes interval, suicide::SuicideStation* suicide)
	: prefix(prefix), alignBed(prefix + "ALIGN"),
	  dropoffTeleporter(prefix + "DROPOFF"),
	  dropoffVault(prefix + "DROPOFF", 350), suicide(suicide)
{
	PopulateGroups(groupedCrates, interval);
	RegisterSlashEvents();
}

void CrateManager::CrateGroupStatistics::AddLooted()
{
	const auto now = std::chrono::system_clock::now();
	std::chrono::seconds elapsedTime =
		(lastLooted != UNDEFINED_TIME)
			? util::GetElapsed<std::chrono::seconds>(lastLooted)
			: std::chrono::minutes(30);

	avgRespawnTime = ((avgRespawnTime * timesLooted) + elapsedTime) /
					 static_cast<int>(timesLooted + 1);

	timesLooted++;
	lastLooted = now;
}

bool CrateManager::Run()
{
	if (!this->crateGroups[0][0].IsReady()) {
		return false;
	}
	auto start = std::chrono::system_clock::now();
	this->SpawnOnAlignBed();

	bool wasAnyLooted = false;
	this->RunAllStations(wasAnyLooted);

	this->TeleportToDropoff();
	if (wasAnyLooted) {
		this->DropoffItems(lastKnownVaultFillLevel);
	}

	this->AccessBedAbove();
	if (this->suicide != nullptr) {
		this->suicide->Complete();
	}

	SendSummaryEmbed(prefix, util::GetElapsed<std::chrono::seconds>(start),
		statisticsPerGroup, lastKnownVaultFillLevel * 100,
		std::chrono::system_clock::now() +
			crateGroups[0][0].GetCompletionInterval());
	return true;
}

void CrateManager::SetGroupCooldown(std::vector<LootCrateStation>& group)
{
	for (auto& station : group) {
		station.SetCanDefaultTeleport(false);
		station.SetCooldown();
	}
}
bool CrateManager::IsReadyToRun() { return false; }

std::chrono::minutes llpp::bots::drops::CrateManager::GetTimeLeftUntilReady()
{
	return std::chrono::minutes(0);
}

void CrateManager::RunAllStations(bool& anyLooted)
{
	anyLooted = false;
	bool canDefaultTeleport = true;

	int i = 0;
	for (auto& group : this->crateGroups) {
		for (auto& station : group) {
			station.SetCanDefaultTeleport(canDefaultTeleport);
			auto result = station.Complete();

			if (result.success) {
				anyLooted = true;
				this->statisticsPerGroup[i].AddLooted();
				this->SetGroupCooldown(group);
				canDefaultTeleport = (&station == &group.back());
				break;
			}
			else {
				canDefaultTeleport = true;
			}
		}
		i++;
	}
}

void CrateManager::DropoffItems(float& filledLevelOut)
{
	asa::entities::gLocalPlayer->TurnUp(50, std::chrono::milliseconds(500));
	asa::entities::gLocalPlayer->TurnRight(90);

	asa::entities::gLocalPlayer->Access(this->dropoffVault);
	asa::entities::gLocalPlayer->inventory->TransferAll();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	filledLevelOut = dropoffVault.GetSlotCount() / 350.f;

	this->dropoffVault.inventory->Close();
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void CrateManager::TeleportToDropoff()
{
	asa::entities::gLocalPlayer->TeleportTo(this->dropoffTeleporter);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	asa::entities::gLocalPlayer->StandUp();
}

void CrateManager::AccessBedAbove()
{
	asa::entities::gLocalPlayer->LookAllTheWayUp();
	asa::entities::gLocalPlayer->Access(this->alignBed);
}

void CrateManager::SpawnOnAlignBed()
{
	asa::entities::gLocalPlayer->FastTravelTo(this->alignBed);
	asa::entities::gLocalPlayer->Crouch();
	asa::entities::gLocalPlayer->TurnDown(20);
}

void CrateManager::PopulateGroups(
	const std::vector<std::vector<QualityFlags>>& groups,
	std::chrono::minutes interval)
{
	crateGroups.resize(groups.size());
	statisticsPerGroup.resize(groups.size());
	int n = 0;

	for (size_t i = 0; i < groups.size(); i++) {
		for (QualityFlags crateQualities : groups[i]) {
			std::string name = util::AddNumberToPrefix(prefix + "DROP", ++n);
			auto station = LootCrateStation(name, crateQualities, interval);
			crateGroups[i].push_back(station);
		}
	}
}

void CrateManager::RegisterSlashEvents()
{
	dpp::slashcommand toggleCompletion(
		"toggle", "Toggle a station on or off", 0);

	toggleCompletion.add_option(
		dpp::command_option(dpp::co_string, "toggle",
			"Whether to enable/disable this station", true)
			.add_choice(dpp::command_option_choice(
				"Paste", std::string("station_paste")))
			.add_choice(dpp::command_option_choice(
				"Crate", std::string("station_crate"))));

	llpp::core::discord::RegisterSlashCommand(
		toggleCompletion, [](const dpp::slashcommand_t& event) {
			auto toggle = std::get<bool>(event.get_parameter("toggle"));
			std::cout << toggle << std::endl;
		});
}