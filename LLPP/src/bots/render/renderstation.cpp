#include "renderstation.h"
#include "../../common/util.h"
#include <asapp/entities/localplayer.h>

using namespace llpp::bots::render;

RenderStation::RenderStation(std::string name, std::chrono::seconds loadFor)
	: renderDuration(loadFor), BaseStation(name, std::chrono::minutes(5)),
	  srcBed(asa::structures::SimpleBed(name)){};

llpp::core::StationResult RenderStation::Complete()
{
	auto start = std::chrono::system_clock::now();

	asa::entities::gLocalPlayer->FastTravelTo(this->srcBed);
	std::this_thread::sleep_for(this->renderDuration);

	asa::entities::gLocalPlayer->FastTravelTo(this->gatewayBed);
	this->SetCompleted();

	auto duration = util::GetElapsed<std::chrono::seconds>(start);
	return core::StationResult(this, true, duration, {});
}