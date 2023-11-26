#pragma once
#include <chrono>
#include <dpp/dpp.h>
#include <iomanip>
#include <string>


namespace llpp::core::discord
{
	bool InitWebhook(const std::string& url);

	inline dpp::webhook* webhook;
	inline dpp::cluster* cl = new dpp::cluster("");

	void Send(const std::string& message);
	void Send(const dpp::embed& embed);
	void InformStarted();
}