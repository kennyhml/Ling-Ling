#pragma once
#include <chrono>
#include <dpp/cluster.h>
#include <dpp/colors.h>
#include <dpp/message.h>
#include <stdexcept>
#include <string>

namespace llpp::core::discord
{
	bool InitWebhooks(const std::string& infoUrl, const std::string& dropUrl);

	inline dpp::webhook* webhook;
	inline dpp::webhook* dropWebhook;

	inline dpp::cluster* cl = new dpp::cluster("");

	void Send(const std::string& message, dpp::webhook*);
	void Send(const dpp::embed&, dpp::webhook*);
	void Send(const dpp::message&, dpp::webhook*);

	void InformStarted();
	void InformFatalError(const std::exception& error, const std::string& task);
}