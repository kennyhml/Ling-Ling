#include "persistenttask.h"
#include "utility/utility.h"

#include <asa/utility.h>
#include <asa/core/logging.h>

namespace lingling
{
    std::filesystem::path get_persistent_task_data_path()
    {
        // try to see if the file exists locally first.
        std::filesystem::path local = std::filesystem::current_path() / "data/data.json";
        if (exists(local)) { return local; }

        // File doesnt exist locally, should be in ProgramData then.
        size_t required_size;
        getenv_s(&required_size, nullptr, 0, "ProgramData");
        if (!required_size) {
            throw std::exception("get_env_s(ProgramData) failed");
        }

        const auto path = static_cast<char*>(malloc(required_size * sizeof(char)));
        getenv_s(&required_size, path, required_size, "ProgramData");
        return std::filesystem::path(path) / "Ling-Ling\\data.json";
    }

    json_t& get_persistent_task_data()
    {
        const auto path = get_persistent_task_data_path();
        if (!exists(path)) {
            create_directories(path.parent_path());
            utility::dump(json_t(), path);
        }

        static json_t data = utility::read(path);
        return data;
    }

    persistent_task::persistent_task(const std::string& t_module, const std::string& t_id,
                                     const std::string& t_description,
                                     const task_priority t_priority)
        : task(t_module, t_id, t_description, t_priority)
    {
        load_from_saved();
    }

    void persistent_task::from_json(const json_t& data)
    {
        state_ = data.value("state", task_state::STATE_UNCHECKED);
        last_completion_ = asa::utility::from_t(data.value("last_completion", 0));
        suspension_start_ = asa::utility::from_t(data.value("suspension_start", 0));
    }

    json_t persistent_task::to_json() const
    {
        json_t ret;
        ret["state"] = state_;
        ret["last_completion"] = std::chrono::system_clock::to_time_t(last_completion_);
        ret["suspension_start"] = std::chrono::system_clock::to_time_t(suspension_start_);
        return ret;
    }

    void persistent_task::dump() const
    {
        json_t data = to_json();
        data["dumped"] = std::time(nullptr);
        get_persistent_task_data()[module_][id_] = data;
        utility::dump(get_persistent_task_data(), get_persistent_task_data_path());
    }

    json_t persistent_task::get_saved_data() const
    {
        return get_persistent_task_data()[module_][id_];
    }

    void persistent_task::load_from_saved()
    {
        const json_t data = get_saved_data();
        if (!data.empty()) { from_json(data); }
    }
}
