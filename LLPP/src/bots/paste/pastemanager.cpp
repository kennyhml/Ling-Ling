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
		stations.push_back(std::make_unique<PasteStation>(name, interval));
	}
};

bool PasteManager::Run()
{
	if (!IsReadyToRun()) {
		return false;
	}

	auto renderStationResult = renderStation.Complete();
	if (!renderStationResult.success) {
		throw std::runtime_error("Render station failed.");
	}

	for (auto& station : stations) {
		station->Complete();
	}
}

bool PasteManager::IsReadyToRun() { return stations[0]->IsReady(); }

std::chrono::minutes PasteManager::GetTimeLeftUntilReady()
{
	const PasteStation* firstStation = stations[0].get();

	auto now = std::chrono::system_clock::now();
	auto timePassed = util::GetElapsed<std::chrono::minutes>(
		firstStation->GetLastCompletion());

	if (timePassed > firstStation->GetCompletionInterval()) {
		return std::chrono::minutes(0);
	}

	return std::chrono::duration_cast<std::chrono::minutes>(
		firstStation->GetCompletionInterval() - timePassed);
}

const PasteStation* PasteManager::PeekStation(int index) const
{
	if (index > 0 && index < stations.size()) {
		return stations[index].get();
	}
	return nullptr;
}
