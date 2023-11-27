#include "pastestationmanager.h"
#include "xexceptions.h"

const bool llpp::bots::paste::PasteStationManager::CompleteReadyStations()
{
	if (!this->IsReadyToRun()) {
		return false;
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