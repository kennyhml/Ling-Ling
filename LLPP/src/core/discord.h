#pragma once
#include <chrono>
#include <dpp/dpp.h>
#include <memory>
#include <stdexcept>
#include <string>

namespace llpp::core::discord
{
	using EventCallback = std::function<void(const dpp::slashcommand_t&)>;

	inline std::unique_ptr<dpp::cluster> bot;
	inline int infoChannelID = 1178195307482325072;

	bool Init(const std::string& token);

	void RegisterSlashCommand(dpp::slashcommand command, EventCallback);

	void InformStarted();
	void InformFatalError(const std::exception& error, const std::string& task);
}