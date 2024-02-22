#include "bedstation.h"
#include "../discord/embeds.h"
#include "../discord/tribelogs/handler.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/tribemanager.h>
#include <asapp/entities/exceptions.h>
#include <asapp/interfaces/spawnmap.h>
#include "../config/config.h"
#include "../discord/bot.h"

namespace llpp::core
{
    BedStation::BedStation(std::string t_name, const std::chrono::minutes t_interval)
        : BaseStation(std::move(t_name), t_interval), spawn_bed_(name_) {}

    BedStation::BedStation(std::string t_name,
                           std::chrono::system_clock::time_point t_last_completed,
                           const std::chrono::minutes t_interval)
        : BaseStation(std::move(t_name), t_last_completed, t_interval), spawn_bed_(name_) {}


    bool BedStation::begin()
    {
        last_started_ = std::chrono::system_clock::now();

        try {
            asa::entities::local_player->fast_travel_to(spawn_bed_, AccessFlags_Default,
                                                        TravelFlags_NoTravelAnimation);
        }
        catch (const asa::entities::FastTravelFailedError& e) {
            std::cerr << e.what() << std::endl;
            asa::entities::local_player->suicide();
            asa::interfaces::spawn_map->spawn_at(reset_bed_.get_name());
            return begin();
        } catch (const asa::interfaces::DestinationNotFound& e) {
            std::cerr << e.what() << std::endl;
            discord::get_bot()->message_create(
                discord::create_station_disabled_message(name_, e.what()));
            set_state(State::DISABLED);
            return false;
        } catch (const asa::interfaces::DestinationNotReady& e) {
            std::cerr << e.what() << std::endl;
            discord::get_bot()->message_create(
                discord::create_station_suspended_message(name_, e.what(),
                                                          std::chrono::minutes(5)));
            suspend_for(std::chrono::minutes(5));
            return false;
        }
        asa::interfaces::tribe_manager->update_tribelogs(discord::handle_tribelog_events);
        asa::core::sleep_for(std::chrono::seconds(1)); // Give logs time to close.
        return true;
    }
}
