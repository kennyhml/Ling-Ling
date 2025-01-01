#include "common.h"
#include "utility/utility.h"

namespace lingling
{

    std::filesystem::path get_config_path()
    {
        // try to see if the file exists locally first.
        std::filesystem::path local = std::filesystem::current_path() / "data\\config.json";
        if (exists(local)) { return local; }

        // File doesnt exist locally, should be in ProgramData then.
        size_t required_size;
        getenv_s(&required_size, nullptr, 0, "ProgramData");
        if (!required_size) {
            throw std::exception("get_env_s(ProgramData) failed");
        }

        const auto path = static_cast<char*>(malloc(required_size * sizeof(char)));
        getenv_s(&required_size, path, required_size, "ProgramData");
        return std::filesystem::path(path) / "Ling-Ling\\config.json";
    }

    json_t& get_config_data()
    {
        static json_t data = utility::read(get_config_path());
        return data;
    }

    void dump_config()
    {
        utility::dump(get_config_data(), get_config_path());
    }
}
