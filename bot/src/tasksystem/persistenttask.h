#pragma once
#include "task.h"
#include "configuration/common.h"

namespace lingling
{
    /**
     * @brief Gets the data path, the persistent task data is to be stored at
     * C:\ProgramData\Ling-Ling\data.json
     *
     * @return The path to the desired data.
     */
    [[nodiscard]] std::filesystem::path get_task_data_path();

    [[nodiscard]] json_t& get_persistent_task_data();

    /**
     * @brief Abstract task base class with data persistence in local files that any task
     * may implement in order to be manageable through the taskmanager.
     *
     * The persistent data is saved in data.json under "tasks".
     */
    class persistent_task : public task
    {
    public:
        persistent_task(const std::string& t_module, const std::string& t_id,
                        const std::string& t_description, task_priority t_priority,
                        std::chrono::minutes t_max_persistence);

        /**
         * @brief Load the data from the provided json data of this object, must be
         * implemented by the subclass to suit all the required data.
         *
         * @param data The data object of this persistent task in data.json.
         */
        virtual void from_json(const json_t& data);

        /**
         * @brief Load the data from the provided json data of this object, must be
         * implemented by the subclass to suit all the required data.
         */
        [[nodiscard]] virtual json_t to_json() const;

        /**
         * @brief Dump the data of the task to data.json in order to make sure it persists
         * across app seessions.
         */
        void dump_data() const;

        [[nodiscard]] json_t get_saved_data() const;

    protected:
        void load_from_saved(std::chrono::minutes max_persistence);
    };
}
