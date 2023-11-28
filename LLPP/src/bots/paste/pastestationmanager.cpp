#include "pastestationmanager.h"
#include "exceptions.h"

const bool llpp::bots::paste::PasteStationManager::CompleteReadyStations()
{
	if (!this->IsReadyToRun()) {
		return false;
	}

	auto renderStationResult = this->renderStation.Complete();
	if (!renderStationResult.success) {
		throw std::runtime_error("Render station failed.");
	}

	for (const auto& station : this->stations) {
		try {
			station->Complete();
		}
		catch (exceptions::PasteNotDeposited e) {
			std::cerr << "[!] Station not completed: " << e.what() << std::endl;
			// TODO: send it to the webhook here
		}
	}
}

const bool llpp::bots::paste::PasteStationManager::IsReadyToRun()
{
	return this->stations[0]->IsReady();
}

std::chrono::minutes
llpp::bots::paste::PasteStationManager::GetTimeLeftUntilReady()
{
	PasteStation* firstStation = this->stations[0].get();

	auto now = std::chrono::system_clock::now();
	auto timePassed = now - firstStation->GetLastCompletion();

	if (timePassed > firstStation->GetCompletionInterval()) {
		return std::chrono::minutes(0);
	}
	return std::chrono::duration_cast<std::chrono::minutes>(
		firstStation->GetCompletionInterval() - timePassed);
}
