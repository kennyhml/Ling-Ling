#include "seedstation.h"

#include "asa/utility.h"
#include "asa/core/logging.h"
#include "asa/core/state.h"
#include "asa/items/items.h"

namespace lingling::gacha
{
    seedstation::seedstation(const std::string& t_name,
                             std::shared_ptr<asa::teleporter> t_teleporter)
        : teleport_station(std::move(t_teleporter)), iguanodon_(t_name) {}

    void seedstation::complete()
    {
        // Let the base take care of teleporting
        // teleport_station::complete();
        const asa::utility::stopwatch sw;

        // We only want to take all from the iguanodon, nothing else
        if (flags_ & SEEDING_INVENTORY_CLEAROUT) {
            take_berries();
            return;
        }

        // Put all into the iguanodon, at this point we should only have berries on us.
        // The iguanodons should not have more than 1450 weight, so the can hold a max.
        // of 145 slots (produce 145 slots of seeds and consume 1 slot of berries)
        if (!(flags_ & SEEDING_NO_BERRY_TRANSFER)) {
            asa::get_local_player()->access(iguanodon_);
            asa::get_local_player()->get_inventory()->transfer_all();
            iguanodon_.get_inventory()->close();
        }

        seed();
        asa::get_local_player()->access(iguanodon_);
        iguanodon_.get_inventory()->transfer_all("seed");

        // we are about to seed on this iguanodon again, so reset its seeding while we
        // already have the inventory open and add the slot that we just lost back in.
        if (flags_ & SEEDING_RESTOCK_FOR_NEXT) {
            asa::get_local_player()->get_inventory()->transfer(1);
        }
        iguanodon_.get_inventory()->close();
        asa::get_logger()->info("Seeding complete ({}ms elapsed).", sw.elapsed().count());
    }

    void seedstation::set_seeding_flags(const seeding_flags_t flags)
    {
        flags_ = flags;
    }

    void seedstation::seed()
    {
        post_press(asa::get_action_mapping("Use"));

        // Iguanodon seems to have a ~500ms grace period where you cant do anything
        // while its doing the seeding animation.
        asa::checked_sleep(500ms);

    }

    void seedstation::take_berries() const
    {
        asa::get_local_player()->access(iguanodon_);
        iguanodon_.get_inventory()->transfer_all();
        iguanodon_.get_inventory()->close();
    }
}
