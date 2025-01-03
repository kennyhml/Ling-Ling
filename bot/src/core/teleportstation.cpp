#include "core/teleportstation.h"

namespace lingling
{
    teleport_station::teleport_station(std::shared_ptr<asa::teleporter> t_teleporter)
        : teleporter_(std::move(t_teleporter)) {}

    void teleport_station::complete()
    {
        asa::get_local_player()->teleport_to(*teleporter_, flags_);
    }

    void teleport_station::set_default_destination(const bool is_default)
    {
        if (is_default) { flags_ |= TeleportFlags_UseDefaultOption; }
        else { flags_ &= static_cast<teleport_flags_t>(~TeleportFlags_UseDefaultOption); }
    }

    void teleport_station::set_unsafe_load(const bool unsafe_load)
    {
        if (unsafe_load) { flags_ |= TeleportFlags_UnsafeLoad; }
        else { flags_ &= static_cast<teleport_flags_t>(~TeleportFlags_UnsafeLoad); }
    }
}
