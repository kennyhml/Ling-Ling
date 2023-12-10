#include "discord.h"
#include "../common/util.h"
#include <asapp/game/window.h>

namespace llpp::core::discord
{
	std::unordered_map<std::string, EventCallback> eventCallbacks;
	std::vector<dpp::slashcommand> commands;

	int infoChannelID = 1178195307482325072;
	bool isInitiliazed = false;

	bool Init(const std::string& token)
	{
		std::cout << "[+] Initializing Ling Ling++ discord bot..." << std::endl;

		infoChannelID = infoChannelID;

		if (token.empty()) {
			std::cerr << "\t[!] Empty token passed!" << std::endl;
			return false;
		}

		std::cout << "\t[-] Creating bot from token... ";
		bot = std::make_unique<dpp::cluster>(token, dpp::i_all_intents);
		std::cout << "Done" << std::endl;

		std::cout << "\t[-] Registering static callbacks... ";
		bot->on_slashcommand(OnSlashCommandCallback);
		bot->on_ready(OnReadyCallback);
		std::cout << "Done" << std::endl;

		std::cout << "\t[-] Discord bot created successfully." << std::endl;
		return true;
	}

	void RegisterSlashCommand(dpp::slashcommand cmd, EventCallback callback)
	{
		std::cout << "[+] Registering slash command: " << cmd.name << "... ";
		commands.push_back(cmd);
		eventCallbacks[cmd.name] = callback;
		std::cout << " Done" << std::endl;
	}

	void OnReadyCallback(const dpp::ready_t& event)
	{
		if (dpp::run_once<struct register_bot_commands>()) {
			std::cout << "[+] Registering discord slash commands... ";

			bot->global_bulk_command_create(commands);
			std::cout << "Done." << std::endl;
		}

		isInitiliazed = true;
	}

	void OnSlashCommandCallback(const dpp::slashcommand_t& event)
	{
		auto& fn = eventCallbacks.at(event.command.get_command_name());
		fn(event);
	}

	void InformStarted()
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

	void InformFatalError(const std::exception& error, const std::string& task)
	{
		dpp::embed embed =
			dpp::embed()
				.set_title("FATAL! Ling Ling++ has crashed!")
				.set_description("Encountered an unexpected error - manual "
								 "restart required.")
				.set_color(dpp::colors::red)
				.add_field("Error: ", error.what(), true)
				.add_field("During Task: ", task, true)
				.set_thumbnail(
					"https://static.wikia.nocookie.net/"
					"arksurvivalevolved_gamepedia/images/1/1c/"
					"Repair_Icon.png/revision/latest?cb=20150731134649")
				.set_image("attachment://image.png");

		auto fileData = util::MatToStringBuffer(asa::window::Screenshot());
		dpp::message message = dpp::message(
			dpp::snowflake(infoChannelID), "<@&1181159721433051136>")
								   .set_allowed_mentions(
									   false, true, false, false, {}, {});
		message.add_file("image.png", fileData, "image/png ").add_embed(embed);

		bot->message_create(message);
	}



}