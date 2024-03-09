#pragma once
#include <asapp/items/items.h>

#define ANY_CRAFTED(name)                                                      \
		asa::items::Item(name, false, asa::items::ItemData::ASCENDANT),        \
		asa::items::Item(name, false, asa::items::ItemData::MASTERCRAFT),      \
		asa::items::Item(name, false, asa::items::ItemData::JOURNEYMAN),       \
		asa::items::Item(name, false, asa::items::ItemData::APPRENTICE),       \
		asa::items::Item(name, false, asa::items::ItemData::RAMSHACKLE),       \
		asa::items::Item(name, false, asa::items::ItemData::PRIMITIVE),

#define ONLY_JM_MC_BPS(name)                                                   \
		asa::items::Item(name, true, asa::items::ItemData::JOURNEYMAN),        \
		asa::items::Item(name, true, asa::items::ItemData::MASTERCRAFT),

#define MC_ASC_CRAFTED_JM_MC_BP(name)                                          \
		asa::items::Item(name, true, asa::items::ItemData::MASTERCRAFT),       \
		asa::items::Item(name, true, asa::items::ItemData::JOURNEYMAN),        \
		asa::items::Item(name, false, asa::items::ItemData::ASCENDANT),        \
		asa::items::Item(name, false, asa::items::ItemData::MASTERCRAFT),

#define ANY_CRAFTED_JM_MC_BP(name)                                             \
	ANY_CRAFTED(name)                                                          \
	ONLY_JM_MC_BPS(name)


namespace llpp::bots::drops
{
    inline std::vector<asa::items::Item> get_priority_order()
    {
        static std::vector<asa::items::Item> priority_order{
            // flak / good weapon bps
            ONLY_JM_MC_BPS("Flak Helmet") ONLY_JM_MC_BPS("Flak Chestpiece")
            ONLY_JM_MC_BPS("Flak Leggings") ONLY_JM_MC_BPS("Flak Gauntlets")
            ONLY_JM_MC_BPS("Flak Boots") ONLY_JM_MC_BPS("Fabricated Sniper Rifle")
            ONLY_JM_MC_BPS("Compound Bow")

            // saddles
            MC_ASC_CRAFTED_JM_MC_BP("Giganotosaurus Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Carcharo " "Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Paracer Platform Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Tusoteuthis Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Basi" "losa" "urus" " Sad" "dle")
            MC_ASC_CRAFTED_JM_MC_BP("Rhyniognatha Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Rex Saddle") MC_ASC_CRAFTED_JM_MC_BP("Stego Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Thylacoleo Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Pteranodon Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Paracer Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Megalodon Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Carbonemys Saddle")
            MC_ASC_CRAFTED_JM_MC_BP("Woolly Rhino Saddle")

            // grindables
            ANY_CRAFTED("Electric Prod") ANY_CRAFTED("Fabricated Sniper " "Rifle")
            ANY_CRAFTED("Compound Bow") ANY_CRAFTED("Pump-Action Shotgun")
            ANY_CRAFTED("Assault Rifle") ANY_CRAFTED("Fabricated Pistol")
            ANY_CRAFTED("Riot Shield") ANY_CRAFTED("Riot Helmet")
            ANY_CRAFTED("Riot Chestpiece") ANY_CRAFTED("Riot Leggings")
            ANY_CRAFTED("Riot Gauntlets") ANY_CRAFTED("Riot Boots")
            ANY_CRAFTED("Scuba Leggings") ANY_CRAFTED("Ghilie Mask")
            ANY_CRAFTED("Ghilie Chestpiece") ANY_CRAFTED("Ghilie " "Leggings")
            ANY_CRAFTED("Ghilie Gauntlets") ANY_CRAFTED("Ghilie Boots")
            ANY_CRAFTED("Harpoon " "Launcher") ANY_CRAFTED("Longneck Rifle")
            ANY_CRAFTED("Simple Pistol") ANY_CRAFTED("Metal" " Sick" "le")
            ANY_CRAFTED("Lance") ANY_CRAFTED("Plesiosaur Saddle") ANY_CRAFTED("Lance")
            ANY_CRAFTED("Plesi" "osaur" " Plat" "form " "Saddl" "e")
            ANY_CRAFTED("Ankylo Saddle") ANY_CRAFTED("Tapejara Saddle")
            ANY_CRAFTED("Kaprosuchus " "Saddle")
            asa::items::Item("Metal " "Arrow", false, asa::items::ItemData::NONE),
            asa::items::Item("Rocket Launcher", false, asa::items::ItemData::NONE),
            asa::items::Item("C4 Detonator"),
            asa::items::Item("C4 Detonator", true, asa::items::ItemData::NONE),
            asa::items::Item("Advanced Sniper Bullet", false, asa::items::ItemData::NONE),
            asa::items::Item("Simple Bullet", false, asa::items::ItemData::NONE),
            asa::items::Item("Rocket Propelled Grenade", true,
                             asa::items::ItemData::NONE),
            asa::items::Item("Rocket Propelled Grenade", false,
                             asa::items::ItemData::NONE),
            asa::items::Item("Simple Shotgun Ammo", false, asa::items::ItemData::NONE),
            asa::items::Item("Poison Grenade", false, asa::items::ItemData::NONE),
        };
        return priority_order;
    }
}
