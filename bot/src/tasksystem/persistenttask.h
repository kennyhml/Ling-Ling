#pragma once
#include "task.h"
#include "configuration/common.h"

namespace lingling
{
    /**
     * @brief Gets the path the persistent task data is to be stored at, normally
     * at C:\ProgramData\Ling-Ling\data.json.
     *
     * @return The path to the desired data.
     */
    [[nodiscard]] std::filesystem::path get_persistent_task_data_path();

    /**
     * @brief Serves as static storage for the persistent task data.
     */
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
                        const std::string& t_description, task_priority t_priority);

        /**
         * @brief Load the data from the provided json data of this object, must be
         * implemented by the subclass to suit all the required data.
         *
         * @param data The data object of this persistent task in data.json.
         *
         * @remark A subclass may override this to extend the amount of state loaded from
         * the configuration, but can still rely on this for the basic loadup.
         */
        virtual void from_json(const json_t& data);

        /**
         * @brief Serialize the state of the task into a persistent json object.
         *
         * @returns A JSON object that holds the required state of the task.
         *
         * @remark A subclass may override this to extend the amount of state serialized
         * into the object, but can still rely on this for the basic serialization.
         */
        [[nodiscard]] virtual json_t to_json() const;

        /**
         * @brief Dump the internal state of the task into the persistent data storage.
         * The state is stored under task module -> task id.
         */
        void dump() const;

    protected:
        /**
         * @brief Gets the json object of the task state from the persistent data storage.
         *
         * @return The json object holding the tasks state, may be empty.
         */
        [[nodiscard]] json_t get_saved_data() const;

        /**
         * @brief Load the task state from the persistent data storage if the max duration
         * has not expired.
         */
        void load_from_saved();
    };
}
