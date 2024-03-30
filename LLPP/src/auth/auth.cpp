#include "auth.h"

#include "../config/config.h"

namespace llpp::auth
{
    namespace
    {
        auto name = skCrypt("LingLing");
        auto ownerid = skCrypt("l6sWjaEeLy");
        auto secret = skCrypt(
                "0196c0ce318b25478144d4b6f99de33546a66928d6240e9369f430a2d8bc4561");
        auto version = skCrypt("1.0");
        auto url = skCrypt("https://keyauth.win/api/1.2/");

        bool first_init = true;

        api* get_app()
        {
            static api app(name.decrypt(), ownerid.decrypt(), secret.decrypt(),
                           version.decrypt(), url.decrypt());
            if (first_init) {
                std::cout << skCrypt("[+] Connecting to server... ").decrypt();
                app.init();
                if (!app.data.success) {
                    throw ConnectionError(
                        "Failed to establish a connection to the server: " + app.data.
                        message);
                }
                std::cout << skCrypt("Done.").decrypt() << std::endl;
                name.clear();
                ownerid.clear();
                secret.clear();
                version.clear();
                url.clear();
                first_init = false;
            }
            return &app;
        }
    }

    void login()
    {
        get_app();
        std::string key;

        if (!config::user::name.get().empty() && !config::user::key.get().empty()) {
            std::cout << skCrypt(
                    "[+] Detected stored login, attempting to auto connect..").decrypt()
                << std::endl;

            get_app()->login(config::user::name.get(), config::user::key.get());
            if (!get_app()->data.success) {
                config::user::name.set("");
                config::user::key.set("");
                std::cout << skCrypt(
                        "\t[-] Login was not successful, stored info deleted.\n").
                    decrypt();
                return login();
            }
            user = config::user::name.get();
            key = config::user::key.get();
        }
        else {
            std::cout << skCrypt("Enter username: ");
            std::cin >> user;
            std::cout << skCrypt("Enter key: ");
            std::cin >> key;

            get_app()->login(user, key);

            if (!get_app()->data.success) {
                std::cout << skCrypt("Invalid license: ") << get_app()->data.message <<
                    std::endl;
                return login();
            }
        }

        std::cout << skCrypt("Hi ").decrypt() << get_app()->data.username << std::endl;
        config::user::name.set(user);
        config::user::key.set(key);
    }
}
