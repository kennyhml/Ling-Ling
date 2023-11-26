#include "webhook.h"


bool llpp::core::discord::InitWebhook(const std::string& url)
{
	std::cout << "[+] Creating webhook from url..." << std::endl;
	if (url.empty()) {
		std::cerr << "\t[!] Failed to create webhook! No URL." << std::endl;
		return false;
	}
	try {
		webhook = new dpp::webhook(url);
	}
	catch (dpp::logic_exception e) {
		std::cerr << "\t[!] Failed to create webhook! " << e.what()
				  << std::endl;
		return false;
	}

	webhook->avatar =
		"https://www.mediastorehouse.com/p/172/"
		"dog-shiba-inu-wearing-oriental-bamboo-straw-24520100.jpg.webp";
	webhook->name = "Ling Ling++";
	std::cout << "[-] Webhook has been created successfully." << std::endl;
	return true;
}

void llpp::core::discord::Send(const std::string& message)
{
	dpp::message msg(message);
	cl->execute_webhook_sync(*webhook, msg);
}

void llpp::core::discord::Send(const dpp::embed& embed)
{
	dpp::message msg = dpp::message();
	msg.add_embed(embed);
	cl->execute_webhook_sync(*webhook, msg);
}


void llpp::core::discord::InformStarted()
{
	auto startedEmbed =
		dpp::embed()
			.set_title("Ling Ling++ is starting...")
			.set_description(
				std::format("Ling Ling++ has been started at <t:{}:t>",
					std::chrono::system_clock::to_time_t(
						std::chrono::system_clock::now())))
			.set_color(dpp::colors::black)
			.add_field("Account: ", "5", true)
			.add_field("Server: ", "SmallTribes9236", true)
			.add_field("Tasks: ", "...", true)
			.set_thumbnail("https://static.wikia.nocookie.net/"
						   "arksurvivalevolved_gamepedia/images/b/b9/"
						   "Whip_%28Scorched_Earth%29.png/revision/latest/"
						   "scale-to-width-down/228?cb=20160901213011")
			.set_footer(
				dpp::embed_footer("Ling Ling++ - please help - @kennyhml"));
	Send(startedEmbed);
}