#include "suicidestation.h"
#include "../../common/util.h"
#include "../../core/basestation.h"
#include "suicideembed.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>
#include <asapp/structures/simplebed.h>

using namespace llpp::bots::suicide;

SuicideStation::SuicideStation(std::string bedName, std::string respawnBedName)
	: BaseStation(bedName, std::chrono::minutes(5)), deathBed(bedName),
	  respawnBed(respawnBedName){};

llpp::core::StationResult SuicideStation::Complete()
{
	auto start = std::chrono::system_clock::now();
	asa::entities::gLocalPlayer->FastTravelTo(this->deathBed);
	asa::entities::gLocalPlayer->Suicide();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	asa::interfaces::gSpawnMap->SpawnAt(this->respawnBed.name);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	this->SetCompleted();

	auto timeTaken = util::GetElapsed<std::chrono::seconds>(start);
	auto data = core::StationResult(this, true, timeTaken, {});
	SendSuicided(data, this->deathBed.name, this->respawnBed.name);
	return data;
}
