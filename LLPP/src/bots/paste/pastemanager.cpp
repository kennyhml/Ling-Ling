#include "pastemanager.h"
#include "../../common/util.h"
#include "embeds.h"
#include <algorithm>
#include <asapp/entities/exceptions.h>

using namespace llpp::bots::paste;

PasteManager::PasteManager(
	std::string prefix, int numStations, std::chrono::minutes interval)
{
	for (int i = 0; i < numStations; i++) {
		std::string name = util::AddNumberToPrefix(prefix, i + 1);
		this->stations.push_back(
			std::make_unique<PasteStation>(name, interval));
	}
};

bool PasteManager::Run()
{
	if (!this->IsReadyToRun()) {
		return false;
	}

	auto renderStationResult = this->renderStation.Complete();
	if (!renderStationResult.success) {
		throw std::runtime_error("Render station failed.");
	}

	for (auto& station : this->stations) {
		station->Complete();
	}
}

bool PasteManager::IsReadyToRun() { return this->stations[0]->IsReady(); }

std::chrono::minutes PasteManager::GetTimeLeftUntilReady()
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