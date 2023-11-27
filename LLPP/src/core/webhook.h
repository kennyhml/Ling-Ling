#pragma once
#include <chrono>
#include <dpp/cluster.h>
#include <dpp/colors.h>
#include <dpp/message.h>

#include <string>

namespace llpp::core::discord
{
	bool InitWebhook(const std::string& url);

	inline dpp::webhook* webhook;
	inline dpp::cluster* cl = new dpp::cluster("");

	void Send(const std::string& message);
	void Send(const dpp::embed&);
	void Send(const dpp::message&);

	void InformStarted();
}