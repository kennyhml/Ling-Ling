#pragma once
#include <chrono>
#include <dpp/dpp.h>
#include <memory>
#include <stdexcept>
#include <string>

namespace llpp::core::discord
{
	std::unique_ptr<dpp::cluster> bot;

	int infoChannelID{};

	bool Init(const std::string& token, int infoChannelId);

	void InformStarted();
	void InformFatalError(const std::exception& error, const std::string& task);
}