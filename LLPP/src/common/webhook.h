#pragma once
#include <string>
#include <dpp/webhook.h>
#include <dpp/message.h>
#include <dpp/cluster.h>


namespace llpp::common::discord
{

	bool InitWebhook(const std::string& url);

	inline dpp::webhook* webhook;
	inline dpp::cluster* cl = new dpp::cluster("");

	void Send(const std::string& message);
	void Send(const dpp::embed& embed);
}