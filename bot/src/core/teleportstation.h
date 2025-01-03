#pragma once
#include <asa/entities/localplayer.h>

namespace lingling
{
    class teleport_station
    {
    public:
        explicit teleport_station(std::shared_ptr<asa::teleporter> t_teleporter);

        /**
         * @brief Completes the teleport station by simply teleporting to it.
         *
         * This behavior allows for convenience on stations that serve no other purpose
         * than being teleported to.
         *
         * Subclasses should override this method to attach more behavior to the station
         * than just teleporting to it.
         */
        virtual void complete();

        /**
         * @brief Sets the teleport flags of this station.
         *
         * @param flags The flags to use when teleporting to this station.
         */
        void set_teleport_flags(const teleport_flags_t flags) { flags_ = flags; }

        /**
         * @brief Sets this station as the default destination of the current teleporter.
         *
         * Setting this flag allows the station to assume that it does not need to go
         * through the teleporter interface.
         *
         * This flag is required to be true when attempting to teleport on a mount.
         *
         * @param is_default Whether this station is the current default destination.
         */
        void set_default_destination(bool is_default);

        /**
         * @brief Sets the unsafe load flag of this station to the desired value
         *
         * Setting this flag allows the station to assume that it has arrived at the
         * destination instantly upon leaving the source teleporter. This is considered
         * unsafe as it could lead to desync behavior if the server is lagging as we tp.
         *
         * When using this flag, you must be cautious to handle any cause for desync
         * yourself once teleported.
         *
         * @param unsafe_load Whether to let the teleport complete unsafe.
         */
        void set_unsafe_load(bool unsafe_load);

    protected:
        virtual ~teleport_station() = default;

        teleport_flags_t flags_ = TeleportFlags_None;
        std::shared_ptr<asa::teleporter> teleporter_;
    };
}
