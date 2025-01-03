#pragma once
#include <asa/structures/simple_bed.h>

namespace lingling
{
    class bed_station
    {
    public:
        explicit bed_station(std::shared_ptr<asa::simple_bed> t_bed);

        /**
         * @brief Completes the station by simply spawning on it.
         *
         * This behavior allows for convenience on stations that serve no other purpose
         * than being spawned on.
         *
         * Subclasses should override this method to attach more behavior to the station
         * than just spawning on it.
         */
        virtual void complete();

    protected:
        virtual ~bed_station() = default;

        asa::simple_bed spawn_bed_;
    };
}
