#include "craftingmanager.h"

#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"

using namespace llpp::config::bots::crafting;

namespace llpp::bots::crafting
{
    namespace
    {
        CraftingManager* instance = nullptr;

        template <typename T>
        void create(std::vector<std::unique_ptr<T>>& into, const int num,
                    const std::string& name, const std::chrono::minutes interval)
        {
            for (int i = 0; i < num; i++) {
                into.push_back(std::make_unique<T>(util::add_num_to_prefix(name, i + 1),
                                                   std::chrono::minutes(interval)));
            }
        }
    }

    CraftingManager::CraftingManager()
    {
        instance = this;
        using namespace llpp::config::bots::crafting;

        create(spark_stations_, spark::num_stations.get(), spark::prefix.get(),
               std::chrono::minutes(spark::interval.get()));
        create(gunpowder_stations_, gunpowder::num_stations.get(),
               gunpowder::prefix.get(), std::chrono::minutes(gunpowder::interval.get()));

        register_slash_commands();
    }

    CraftingManager::~CraftingManager() { instance = nullptr; }

    bool CraftingManager::run()
    {
        if (!is_ready_to_run()) { return false; }
        run_spark();
        run_gunpowder();

        return true;
    }

    void CraftingManager::run_spark() const
    {
        if (spark::disabled.get()) { return; }
        for (const auto& station : spark_stations_) {
            try { if (station->is_ready()) { station->complete(); } }
            catch (const StationFullError&) {
                send_station_capped(station->get_name(), station->get_last_dedi_ss());
                station->set_disabled(true);
            }
        }
    }

    void CraftingManager::run_gunpowder() const
    {
        if (gunpowder::disabled.get()) { return; }
        for (const auto& station : gunpowder_stations_) {
            try { if (station->is_ready()) { station->complete(); } }
            catch (const StationFullError&) {
                send_station_capped(station->get_name(), station->get_last_dedi_ss());
                station->set_disabled(true);
            }
        }
    }

    std::chrono::minutes CraftingManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            spark_stations_[0]->get_next_completion());
    }

    bool CraftingManager::is_ready_to_run() const
    {
        using namespace config::bots::crafting;

        return (!spark::disabled.get() && spark_stations_[0]->is_ready()) || (!
            gunpowder::disabled.get() && gunpowder_stations_[0]->is_ready());
    }

    void CraftingManager::register_slash_commands()
    {
        if (has_registered_commands_) { return; }

        dpp::slashcommand crafting("crafting", "Controls the crafting manager", 0);

        dpp::command_option enable(dpp::co_sub_command, "enable",
                                   "Enable completion of a managed crafting station.");

        dpp::command_option enable_field(dpp::co_string, "station",
                                         "The station to enable", true);

        for (const auto& station : instance->spark_stations_) {
            std::cout << station->get_name();
            enable_field.add_choice({station->get_name(), station->get_name()});
        }
        for (const auto& station : instance->gunpowder_stations_) {
            std::cout << station->get_name();
            enable_field.add_choice({station->get_name(), station->get_name()});
        }

        dpp::command_option disable(dpp::co_sub_command, "disable",
                                    "Disable completion of a managed crafting station.");

        auto disable_field = enable_field;
        disable_field.description = "The station to disable";

        crafting.add_option(enable.add_option(enable_field));
        crafting.add_option(disable.add_option(disable_field));


        core::discord::register_slash_command(crafting, slashcommand_callback);
        has_registered_commands_ = true;
    }

    void CraftingManager::slashcommand_callback(const dpp::slashcommand_t& event)
    {
        if (core::discord::handle_unauthorized_command(event)) { return; }

        auto cmd_options = event.command.get_command_interaction();

        auto subcommand = cmd_options.options[0];
        if (subcommand.name == "enable") {
            auto station_name = subcommand.get_value<std::string>(0);

            for (const auto& station : instance->spark_stations_) {
                if (station->get_name() == station_name) {
                    if (!station->get_disabled()) {
                        return event.reply(std::format(
                            "Sparkpowder Station '{}' is already enabled.",
                            station_name));
                    }
                    station->set_disabled(false);
                    return event.reply(std::format(
                        "Sparkpowder Station '{}' has been enabled.", station_name));
                }
            }
            for (const auto& station : instance->gunpowder_stations_) {
                if (station->get_name() == station_name) {
                    if (!station->get_disabled()) {
                        return event.reply(std::format(
                            "Gunpowder Station '{}' is already enabled.", station_name));
                    }
                    station->set_disabled(false);
                    return event.reply(std::format(
                        "Gunpowder Station '{}' has been enabled.", station_name));
                }
            }
            return event.reply(std::format("No station named '{}' was found.",
                                           station_name));
        }
        if (subcommand.name == "disable") {
            auto name = subcommand.get_value<std::string>(0);

            for (const auto& station : instance->spark_stations_) {
                if (station->get_name() == name) {
                    if (station->get_disabled()) {
                        return event.reply(std::format(
                            "Sparkpowder Station '{}' is already disabled.", name));
                    }
                    station->set_disabled(true);
                    return event.reply(
                        std::format("Sparkpowder Station '{}' has been disabled.", name));
                }
            }
            for (const auto& station : instance->gunpowder_stations_) {
                if (station->get_name() == name) {
                    if (station->get_disabled()) {
                        return event.reply(std::format(
                            "Gunpowder Station '{}' is already disabled.", name));
                    }
                    station->set_disabled(true);
                    return event.reply(
                        std::format("Gunpowder Station '{}' has been disabled.", name));
                }
            }
            return event.reply(std::format("No station named '{}' was found.", name));
        }
    }
}
