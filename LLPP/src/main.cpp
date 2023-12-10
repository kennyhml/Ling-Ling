
#include "bots/drops/cratemanager.h"
#include "bots/drops/lootcratestation.h"
#include "bots/paste/pastemanager.h"
#include "bots/suicide/suicidestation.h"
#include "core/recovery.h"
#include "core/webhook.h"
#include <asapp/core/exceptions.h>
#include <asapp/entities/localplayer.h>

#include <asapp/core/init.h>
#include <asapp/entities/exceptions.h>
#include <asapp/interfaces/serverselect.h>
#include <dpp/dpp.h>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
	asa::Init(std::filesystem::path("src/config.json"));

	std::ifstream f("src/config.json");
	json data = json::parse(f);
	f.close();


	dpp::cluster bot(
		data["bot"], dpp::i_default_intents | dpp::i_message_content);


	bot.on_message_create([&bot](const dpp::message_create_t& event) {
		std::cout << event.msg.content << std::endl;
	});


	bot.on_slashcommand([](const dpp::slashcommand_t& event) {
		if (event.command.get_command_name() == "ping") {
			event.reply("Nigga!");
		}
	});

	bot.on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			bot.global_command_create(
				dpp::slashcommand("ping", "Nigga nig pong!", bot.me.id));
		}
	});


	bot.message_create(
		dpp::message(dpp::snowflake(1178195307482325072), "Test"));

	bot.start(dpp::st_wait);

	std::cout << "Still running" << std::endl;

	asa::window::GetHandle(60, true);
	asa::window::SetForeground();

	using Quality = asa::structures::CaveLootCrate::Quality;

	auto suicideStation = llpp::bots::suicide::SuicideStation(
		"SUICIDE DEATH", "SUICIDE RESPAWN");

	auto swamp = llpp::bots::drops::CrateManager("SWAMP::",
		{ { Quality::RED, Quality::RED },
			{ Quality::YELLOW, Quality::YELLOW, Quality::ANY },
			{ Quality::BLUE } },
		std::chrono::minutes(5), &suicideStation);

	auto paste = llpp::bots::paste::PasteManager(
		"PASTE", 6, std::chrono::minutes(50));
	auto skylord = llpp::bots::drops::CrateManager("SKYLORD::",
		{
			{ Quality::YELLOW | Quality::RED, Quality::YELLOW | Quality::RED,
				Quality::YELLOW | Quality::RED },
		},
		std::chrono::minutes(5));

	while (true) {
		try {
			if (swamp.Run())
				continue;
			if (skylord.Run())
				continue;
			if (paste.Run())
				continue;


			std::cout << "No task ready...." << std::endl;
		}
		catch (asa::exceptions::ShooterGameError& e) {
			llpp::core::InformCrashDetected(e);
			llpp::core::Recover();
		}
		catch (const std::exception& e) {
			llpp::core::discord::InformFatalError(e, "???");
			break;
		}
	}
	return 0;
}