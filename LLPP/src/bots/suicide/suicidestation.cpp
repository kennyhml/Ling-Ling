#include "suicidestation.h"
#include "../../core/basestation.h"
#include "suicideembed.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/spawnmap.h>
#include <asapp/structures/simplebed.h>


const llpp::core::StationResult llpp::bots::suicide::SuicideStation::Complete()
{
	auto start = std::chrono::system_clock::now();
	asa::entities::gLocalPlayer->FastTravelTo(&this->deathBed);
	asa::entities::gLocalPlayer->Suicide();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	asa::interfaces::gSpawnMap->SpawnAt(this->respawnBed.name);
	std::this_thread::sleep_for(std::chrono::seconds(10));

	this->SetCompleted();
	auto now = std::chrono::system_clock::now();
	auto timeTaken = std::chrono::duration_cast<std::chrono::seconds>(
		now - start);


	auto data = core::StationResult(
		this, true, this->GetTimesCompleted(), timeTaken, {});

	SendSuicided(data, this->deathBed.name, this->respawnBed.name);
	return data;
}
