#pragma once
#include <asapp/entities/dinoent.h>
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/simplebed.h>
#include <dpp/user.h>

namespace llpp::bots::farm
{
    bool run_while_requested();

    class FarmBot
    {
    public:
        static FarmBot* instance() { return inst_; }

        enum FarmType
        {
            METAL,
            WOOD,
        };

    public:
        explicit FarmBot(FarmType t_type, const std::string& t_where,
                         const dpp::user& t_with_who);
        ~FarmBot() { inst_ = nullptr; }

        /**
         * @brief Runs the farmbot instance until it is terminated or an error occurs. 
         */
        void run();

        /**
         * @brief Tells the farm bot thread to terminate.
         */
        void signal_end() { stop_requested_ = true; }

        /**
         * @brief Destroys the farming session and resets the singleton state. 
         */
        void destroy();

        /**
         * @brief Gets the discord user that requested to start this farm bot instance. 
         */
        [[nodiscard]] const dpp::user& get_user() const { return requester_; }

        /**
         * @brief Gets the type of resource the bot should be farming.
         */
        [[nodiscard]] FarmType get_farm_type() const { return resource_; }

        /**
         * @brief Checks whether the farm bot has been terminated.
         */
        [[nodiscard]] bool has_ended() const { return stop_requested_; }

        /**
         * @brief Checks whether the farm bot session has been started.
         */
        [[nodiscard]] bool has_started() const { return has_started_; }

    private:
        void drop_trash();
        void help_popcorn(asa::items::Item& item);

        inline static FarmBot* inst_ = nullptr;

        bool stop_requested_ = false;
        bool has_started_ = false;

        dpp::user requester_;
        FarmType resource_;

        asa::structures::SimpleBed spawn_at_;

        asa::entities::DinoEnt mount_;
    };
}
