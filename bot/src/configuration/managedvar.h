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

        T get()
        {
            auto str = data_.dump(4);
            if (!loaded_) {
                value_ = json_traits_.from_json(data_);
                loaded_ = true;
            }
            return value_;
        }

        T* get_ptr()
        {
            get();
            return &value_;
        }

        T& get_ref()
        {
            get();
            return value_;
        }

        void set(const T& new_value, const bool dump = true)
        {
            value_ = new_value;
            if (dump) { sync_to_file(); }
        }

        void sync_to_file()
        {
            data_ = json_traits_.to_json(value_);
            dump_();
        }

        void sync_from_file()
        {
            loaded_ = false;
            get();
        }

    private:
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
