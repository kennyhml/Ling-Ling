#include "renderstation.h"
#include <asapp/entities/localplayer.h>

const llpp::core::StationResult llpp::bots::render::RenderStation::Complete()
{
	auto start = std::chrono::system_clock::now();

	asa::entities::gLocalPlayer->FastTravelTo(this->srcBed);
	std::this_thread::sleep_for(this->renderDuration);

	asa::entities::gLocalPlayer->FastTravelTo(this->gatewayBed);
	this->SetCompleted();

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now() - start);

	return core::StationResult(
		this, true, this->GetTimesCompleted(), duration, {});
}