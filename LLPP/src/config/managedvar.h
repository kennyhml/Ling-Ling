#pragma once
#include <iostream>
#include <dpp/json.h>
#include <mutex>

template <typename T>
static std::vector<T> extend(std::vector<T> base, T element)
{
    base.push_back(element);
    return base;
}

namespace llpp::config
{
    inline std::mutex save_mutex;

	nlohmann::ordered_json& get_data();

    template <typename T>
    struct ManagedVar
    {
    public:
        explicit ManagedVar(const std::vector<std::string>& t_var_path,
                            const std::function<void()>& t_on_change,
                            T t_default) : default_(t_default), path_(t_var_path),
                                           on_change_(t_on_change) {};

        explicit ManagedVar(const std::vector<std::string>& t_base, std::string t_key,
                            const std::function<void()>& t_on_change,
                            T t_default) : ManagedVar(extend(t_base, std::move(t_key)),
                                                      t_on_change, t_default) {}

        ManagedVar() = default;
        ~ManagedVar() = default;

        T get();
        T* get_ptr()
        {
            if (!initial_loaded_) { get(); }
            return &value_;
        }

        void set(const T& value);
        void save();
        void erase();

    private :
        T value_;
        T default_;
        bool has_inserted_default_ = false;
        bool initial_loaded_ = false;
        std::vector<std::string> path_;
        std::function<void()> on_change_;

		nlohmann::ordered_json& walk_json(nlohmann::ordered_json& data, bool create_if_not_exist = false) const;
        void handle_not_found(const nlohmann::ordered_json::out_of_range& e);
    };
}
