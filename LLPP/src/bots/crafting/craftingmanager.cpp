#include "craftingmanager.h"

#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../discord/bot.h"

using namespace llpp::config::bots::crafting;

namespace llpp::bots::crafting
{
    namespace
    {
        CraftingManager* instance = nullptr;

        template<typename T>
        void create(std::vector<std::unique_ptr<T>>& into, const int num,
                    const std::string& name, const std::chrono::minutes interval)
        {
            for (int i = 0; i < num; i++) {
                into.push_back(std::make_unique<T>(util::add_num_to_prefix(name, i + 1),
                                                   std::chrono::minutes(interval)));
            }
        }

        template<typename T>
        void add_station_choices(
                const std::vector<std::unique_ptr<T>>& stations,
                dpp::command_option& field)
        {
            for (const auto& station: stations) {
                field.add_choice({station->get_name(), station->get_name()});
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

        create(grinding_stations_, grinding::num_stations.get(),
               grinding::prefix.get(), std::chrono::minutes(grinding::interval.get()));
        create(forge_stations_, forges::num_stations.get(),
               forges::prefix.get(), std::chrono::minutes(forges::interval.get()));
        create(arb_stations_, arb::num_stations.get(),
               arb::prefix.get(), std::chrono::minutes(arb::interval.get()));
        register_slash_commands();
    }

    CraftingManager::~CraftingManager() { instance = nullptr; }

    bool CraftingManager::run()
    {
        if (!is_ready_to_run()) { return false; }
        run_spark();
        run_gunpowder();
        run_grinding();
        run_forges();
        run_arb();

        return true;
    }

    void CraftingManager::run_spark() const
    {
        if (spark::disabled.get()) { return; }
        for (const auto& station: spark_stations_) {
            try { if (station->is_ready()) { station->complete(); }}
            catch (const StationFullError&) {
                send_station_capped(station->get_name(), station->get_last_dedi_ss());
                station->set_state(core::BaseStation::State::DISABLED);
            }
        }
    }

    void CraftingManager::run_gunpowder() const
    {
        if (gunpowder::disabled.get()) { return; }
        for (const auto& station: gunpowder_stations_) {
            try { if (station->is_ready()) { station->complete(); }}
            catch (const StationFullError&) {
                send_station_capped(station->get_name(), station->get_last_dedi_ss());
                station->set_state(core::BaseStation::State::DISABLED);
            }
        }
    }

    void CraftingManager::run_grinding() const
    {
        if (grinding::disabled.get()) { return; }
        for (const auto& station: grinding_stations_) {
            if (station->is_ready()) { station->complete(); }
        }
    }

    void CraftingManager::run_forges() const
    {
        if (forges::disabled.get()) { return; }
        for (const auto& station: forge_stations_) {
            if (station->is_ready()) { station->complete(); }
        }
    }

    void CraftingManager::run_arb() const
    {
        if (arb::disabled.get()) { return; }
        for (const auto& station: arb_stations_) {
            try { if (station->is_ready()) { station->complete(); }}
            catch (const StationFullError&) {
                send_station_capped(station->get_name(), station->get_last_dedi_ss());
                station->set_state(core::BaseStation::State::DISABLED);
            }
        }
    }

    bool CraftingManager::is_spark_ready() const
    {
        if (spark::disabled.get() || spark_stations_.empty()) { return false; }
        // TODO: Allow partial completion by checking if any is ready
        return spark_stations_[0]->is_ready();
    }

    bool CraftingManager::is_gunpowder_ready() const
    {
        if (gunpowder::disabled.get() || gunpowder_stations_.empty()) { return false; }
        // TODO: Allow partial completion by checking if any is ready
        return gunpowder_stations_[0]->is_ready();
    }

    bool CraftingManager::is_grinding_ready() const
    {
        if (grinding::disabled.get() || grinding_stations_.empty()) { return false; }
        // TODO: Allow partial completion by checking if any is ready
        return grinding_stations_[0]->is_ready();
    }

    bool CraftingManager::is_forges_ready() const
    {
        if (forges::disabled.get() || forge_stations_.empty()) { return false; }
        // TODO: Allow partial completion by checking if any is ready
        return forge_stations_[0]->is_ready();
    }

    bool CraftingManager::is_arb_ready() const
    {
        if (arb::disabled.get() || arb_stations_.empty()) { return false; }
        // TODO: Allow partial completion by checking if any is ready
        return arb_stations_[0]->is_ready();
    }


    core::BaseStation* CraftingManager::find_station(const std::string& name) const
    {
        for (const auto& station: spark_stations_) {
            if (station->get_name() == name) { return station.get(); }
        }
        for (const auto& station: gunpowder_stations_) {
            if (station->get_name() == name) { return station.get(); }
        }
        for (const auto& station: grinding_stations_) {
            if (station->get_name() == name) { return station.get(); }
        }
        for (const auto& station: forge_stations_) {
            if (station->get_name() == name) { return station.get(); }
        }
        for (const auto& station: arb_stations_) {
            if (station->get_name() == name) { return station.get(); }
        }
        return nullptr;
    }

    std::chrono::minutes CraftingManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
                spark_stations_[0]->get_next_completion());
    }

    bool CraftingManager::is_ready_to_run()
    {
        return is_spark_ready() || is_gunpowder_ready() || is_grinding_ready() ||
               is_forges_ready() || is_arb_ready();
    }

    void CraftingManager::register_slash_commands()
    {
        if (has_registered_commands_) { return; }

        dpp::slashcommand crafting("crafting", "Controls the crafting manager",
                                   0);

        dpp::command_option enable(dpp::co_sub_command,
                                   "enable",
                                   "Enable completion of a managed crafting station.");

        dpp::command_option enable_field(dpp::co_string,
                                         "station",
                                         "The station to enable", true);

        add_station_choices(spark_stations_, enable_field);
        add_station_choices(gunpowder_stations_, enable_field);
        add_station_choices(grinding_stations_, enable_field);
        add_station_choices(forge_stations_, enable_field);
        add_station_choices(arb_stations_, enable_field);

        dpp::command_option disable(dpp::co_sub_command,
                                    "disable",
                                    "Disable completion of a managed crafting station.");

        auto disable_field = enable_field;
        disable_field.description = "The station to disable";

        crafting.add_option(enable.add_option(enable_field));
        crafting.add_option(disable.add_option(disable_field));


        discord::register_slash_command(crafting, slashcommand_callback);
        has_registered_commands_ = true;
    }

    void CraftingManager::slashcommand_callback(const dpp::slashcommand_t& event)
    {
        if (discord::handle_unauthorized_command(event)) { return; }

        auto cmd_options = event.command.get_command_interaction();
        auto subcommand = cmd_options.options[0];
        const auto name = subcommand.get_value<std::string>(0);
        core::BaseStation* station = instance->find_station(name);

        if (!station) {
            return event.reply(std::format("No station named '{}' was found.", name));
        }

        if (subcommand.name == "enable") {
            if (station->get_state() == core::BaseStation::State::ENABLED) {
                return event.reply(std::format("'{}' is already enabled.", name));
            }
            station->set_state(core::BaseStation::State::ENABLED);
            return event.reply(std::format("'{}' has been enabled.", name));
        } else if (subcommand.name == "disable") {
            if (station->get_state() == core::BaseStation::State::DISABLED) {
                return event.reply(std::format("'{}' is already enabled.", name));
            }
            station->set_state(core::BaseStation::State::DISABLED);
            return event.reply(std::format("'{}' has been disabled.", name));
        }
    }
}
