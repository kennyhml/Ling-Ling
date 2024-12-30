#include "discord.h"

#include <iostream>

#include "config.h"

namespace lingling::discord
{
    void startup()
    {
        if (bot_token.get().empty()) {
            std::string token;
            std::cout << "Discord Bot Token: ";
            std::getline(std::cin, token);

            bot_token.set(token);
        }

        if (guild_id.get().empty()) {
            std::string id;
            std::cout << "Discord Server ID: ";
            std::getline(std::cin, id);

            guild_id.set(id);
        }
    }
}
