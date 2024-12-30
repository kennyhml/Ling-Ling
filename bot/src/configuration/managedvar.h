#pragma once
#include <nlohmann/json.hpp>

namespace config
{
    using json_t = nlohmann::ordered_json;

    /**
     * @brief JSON traits to implement serialization between custom config objects and
     * their json serialized form to enable conversion back and forth.
     *
     * @tparam T The type the trait is converting.
     */
    template<typename T>
    struct json_traits
    {
        using to_json_t = std::function<json_t(const T&)>;
        using from_json_t = std::function<T(const json_t&)>;

        to_json_t to_json = [](const T& value) { return json_t(value); };
        from_json_t from_json = [](const json_t& json) { return json.get<T>(); };
    };

    // Function to call when the config should be dumped (in its entirety)
    using config_dump_t = std::function<void()>;

    /**
     * @brief Manages a variable that is synced internally and serialized in a file.
     *
     * Through the json traits, custom serialization can be enabled to, for example,
     * serialize a whole struct at once.
     *
     * @tparam T The type of the variable that is being managed.
     */
    template<typename T>
    class managed_var
    {
    public:
        managed_var(std::string t_key, json_t& t_data, config_dump_t t_dump,
                    T t_default_value, json_traits<T> t_traits = {})
            : key_(std::move(t_key)), json_traits_(std::move(t_traits)),
              dump_(std::move(t_dump)), default_(std::move(t_default_value)),
              data_(get_or_create(t_data)) {}

        // Allow implicit casting to the managed type
        [[nodiscard]] operator T() { return get(); }

        /**
         * @brief Get the value of this variable.
         *
         * @remark If you need a reload from the object, call @link sync_from_obj \endlink first.
         *
         * @return The value of the variable.
         */
        T get()
        {
            if (!loaded_) {
                value_ = json_traits_.from_json(data_);
                loaded_ = true;
            }
            return value_;
        }

        /**
         * @brief Gets a pointer to this variable.
         *
         * @remark If you need a reload from the object, call @link sync_to_obj \endlink first.
         *
         * @return A pointer to the variable.
         */
        T* get_ptr()
        {
            get();
            return &value_;
        }

        /**
         * @brief Gets a reference to this variable.
         *
         * @remark If you need a reload from the object, call @link sync_to_obj \endlink first.
         *
         * @return A reference to the variable.
         */
        T& get_ref()
        {
            get();
            return value_;
        }

        /**
         * @brief Sets the value of this variable.
         *
         * @param new_value The new value of the variable.
         * @param dump Whether to dump the data to the file after setting the new value.
         */
        void set(const T& new_value, const bool dump = true)
        {
            value_ = new_value;
            sync_to_obj(dump);
        }

        /**
         * @brief Syncs the value of this variable to the json object.
         *
         * @param with_dump Whether to dump the data to the json file after syncing it.
         */
        void sync_to_obj(const bool with_dump = true)
        {
            data_ = json_traits_.to_json(value_);
            if (with_dump) { dump_(); }
        }

        /**
         * @brief Syncs the value of this variable from the json object.
         */
        void sync_from_obj()
        {
            loaded_ = false;
            get();
        }

    private:
        /**
         * @brief Wrapper to get the reference to the json object that is being handled
         * or create it with the default value if it does not yet exist.
         *
         * @param data The json data reference that the desired reference exists in.
         *
         * @remark The key, json traits and default value must be initialized.
         *
         * @return Reference to the json object that this managed_var manages.
         */
        json_t& get_or_create(json_t& data)
        {
            try {
                return data.at(key_);
            } catch (const json_t::out_of_range&) {
                // Add the key to the json with the default value if it didnt exist
                return data[key_] = json_traits_.to_json(default_);
            }
        }

        std::string key_;

        json_traits<T> json_traits_;
        config_dump_t dump_;

        T value_;
        T default_;
        json_t& data_;
        bool loaded_ = false;
    };
}
