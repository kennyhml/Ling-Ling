#include "pastestationmanager.h"
#include "../../common/util.h"
#include <algorithm>
#include <asapp/entities/exceptions.h>


using namespace llpp::bots::paste;

PasteStationManager::PasteStationManager(
	std::string prefix, int numOfStations, std::chrono::minutes interval)
	: BaseStationManager(prefix, numOfStations)
{
	for (int i = 0; i < numOfStations; i++) {
		std::string name = this->CreateStationName(prefix, i + 1);
		this->stations.push_back(
			std::make_unique<PasteStation>(name, interval));
	}
};

bool PasteStationManager::CompleteReadyStations()
{
	if (!this->IsReadyToRun()) {
		return false;
	}

	auto renderStationResult = this->renderStation.Complete();
	if (!renderStationResult.success) {
		throw std::runtime_error("Render station failed.");
	}

	for (auto& station : this->stations) {
		try {
			station->Complete();
		}
		catch (asa::entities::exceptions::EntityNotAccessed& e) {
			std::cerr << "[!] Station not completed: " << e.what() << std::endl;
		}
	}
}

bool PasteStationManager::IsReadyToRun()
{
	return this->stations[0]->IsReady();
}

std::chrono::minutes PasteStationManager::GetTimeLeftUntilReady()
{
	const PasteStation* firstStation = this->stations[0].get();

	auto now = std::chrono::system_clock::now();
	auto timePassed = util::GetElapsed<std::chrono::minutes>(
		firstStation->GetLastCompletion());

	if (timePassed > firstStation->GetCompletionInterval()) {
		return std::chrono::minutes(0);
	}

	return std::chrono::duration_cast<std::chrono::minutes>(
		firstStation->GetCompletionInterval() - timePassed);
}