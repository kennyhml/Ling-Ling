#include "discord.h"
#include "../common/util.h"
#include <asapp/game/window.h>

bool llpp::core::discord::Init(const std::string& token, int infoChannelId)
{
	std::cout << "[+] Initializing Ling Ling++ discord bot..." << std::endl;

	infoChannelID = infoChannelID;

	if (token.empty()) {
		std::cerr << "\t[!] Empty token passed!" << std::endl;
		return false;
	}

	std::cout << "\t[-] Creating bot from token... ";
	bot = std::make_unique<dpp::cluster>(token, dpp::i_all_intents);
	std::cout << " Done" << std::endl;

	bot->on_ready([](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			std::cout << "\t[-] Registering slash commands... ";
			bot->global_command_create(
				dpp::slashcommand("ping", "Nigga nig pong!", bot->me.id));

			std::cout << "Done." << std::endl;
		}
	});

	std::cout << "\t[-] Discord bot created successfully." << std::endl;
	return true;
}

void llpp::core::discord::InformStarted()
{
	dpp::embed startedEmbed =
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

	bot->message_create(
		dpp::message(dpp::snowflake(infoChannelID), startedEmbed));
}

void llpp::core::discord::InformFatalError(
	const std::exception& error, const std::string& task)
{
	dpp::embed embed =
		dpp::embed()
			.set_title("FATAL! Ling Ling++ has crashed!")
			.set_description(
				"Encountered an unexpected error - manual restart required.")
			.set_color(dpp::colors::red)
			.add_field("Error: ", error.what(), true)
			.add_field("During Task: ", task, true)
			.set_thumbnail("https://static.wikia.nocookie.net/"
						   "arksurvivalevolved_gamepedia/images/1/1c/"
						   "Repair_Icon.png/revision/latest?cb=20150731134649")
			.set_image("attachment://image.png");

	auto fileData = util::MatToStringBuffer(asa::window::Screenshot());
	dpp::message message =
		dpp::message(dpp::snowflake(infoChannelID), "<@&1181159721433051136>")
			.set_allowed_mentions(false, true, false, false, {}, {});
	message.add_file("image.png", fileData, "image/png ").add_embed(embed);

	bot->message_create(message);
}
