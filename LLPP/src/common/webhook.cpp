#include "webhook.h"


bool llpp::common::discord::InitWebhook(const std::string& url)
{
	std::cout << "[+] Creating webhook for url: " << url << "..." << std::endl;
	if (url.empty()) {
		std::cerr << "\t[!] Failed to create webhook! URL is empty." << std::endl;
		return false;
	}
	try {
		webhook = new dpp::webhook(url);
	}
	catch (dpp::logic_exception e) {
		std::cerr << "\t[!] Failed to create webhook! " << e.what() << std::endl;
		return false;
	}

	webhook->avatar = "https://www.mediastorehouse.com/p/172/dog-shiba-inu-wearing-oriental-bamboo-straw-24520100.jpg.webp";
	webhook->name = "Ling Ling++";
	std::cout << "[-] Webhook has been created successfully." << std::endl;
	return true;

}

void llpp::common::discord::Send(const std::string& message)
{
	dpp::message msg(message);
	cl->execute_webhook_sync(*webhook, msg);
}

void llpp::common::discord::Send(const dpp::embed& embed)
{
	dpp::message msg;
	msg.embeds.push_back(embed);
	cl->execute_webhook_sync(*webhook, msg);
}

