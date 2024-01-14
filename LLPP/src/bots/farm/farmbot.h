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

        void run();

        void signal_end() { stop_requested_ = true; }
        [[nodiscard]] const dpp::user& get_user() const { return requester_; }
        [[nodiscard]] FarmType get_farm_type() const { return resource_; }

        [[nodiscard]] bool has_ended() const { return stop_requested_; }

    private:
        void drop_trash();
        void help_popcorn(asa::items::Item& item);

        inline static FarmBot* inst_ = nullptr;

        bool stop_requested_ = false;
        dpp::user requester_;
        FarmType resource_;

        asa::structures::SimpleBed spawn_at_;

        asa::entities::DinoEnt mount_;
    };
}
