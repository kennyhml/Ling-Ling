#include "managedvar.h"
#include "../bots/drops/parameters.h"

namespace llpp::config
{
    template <typename T>
    T ManagedVar<T>::get()
    {
        std::lock_guard<std::mutex> guard(save_mutex);
        if (initial_loaded_) { return value_; }
        initial_loaded_ = true;

        try {
            json& curr = walk_json(get_data());
            value_ = curr.at(path_.back()).get<T>();
        }
        catch (const json::out_of_range& e) { handle_not_found(e); }
        return value_;
    }

    template <>
    const char* ManagedVar<const char*>::get()
    {
        std::lock_guard<std::mutex> guard(save_mutex);
        if (initial_loaded_) { return value_; }
        initial_loaded_ = true;

        try {
            json& curr = walk_json(get_data());
            // need to copy the const char to our value, otherwise itll get deleted
            // after we return the pointer to it.
            value_ = _strdup(curr.at(path_.back()).get<std::string>().c_str());
        }
        catch (const json::out_of_range& e) { handle_not_found(e); }
        return value_;
    }

    template <>
    std::vector<const char*> ManagedVar<std::vector<const char*>>::get()
    {
        std::lock_guard<std::mutex> guard(save_mutex);
        if (initial_loaded_) { return value_; }
        initial_loaded_ = true;

        try {
            json& curr = walk_json(get_data());
            // Need to copy each element into our value individually.
            const auto& array = curr.at(path_.back());
            value_.clear();
            for (const auto& element : array) {
                value_.emplace_back(_strdup(element.get<std::string>().c_str()));
            }
        }
        catch (const json::out_of_range& e) { handle_not_found(e); }
        return value_;
    }

    template <>
    bots::drops::CrateManagerConfig ManagedVar<bots::drops::CrateManagerConfig>::get()
    {
        std::lock_guard<std::mutex> guard(save_mutex);

        if (initial_loaded_) { return value_; }
        initial_loaded_ = true;

        try {
            json& curr = walk_json(get_data());
            const std::string k = path_.back();

            if (!curr.contains(k)) { set(default_); }

            std::cout << curr.dump(4) << "\n";

            value_.prefix = curr[k].value("prefix", default_.prefix);
            value_.grouped_crates_raw = curr[k].value("grouped_crates_raw",
                                                      default_.grouped_crates_raw);
            value_.interval = curr[k].value("interval", default_.interval);
            value_.render_for = curr[k].value("render_for", default_.render_for);
            value_.uses_teleporters = curr[k].value("uses_teleporters",
                                                    default_.uses_teleporters);
            value_.overrule_reroll_mode = curr[k].value(
                "overrule_reroll_mode", default_.overrule_reroll_mode);
            value_.allow_partial_completion = curr[k].value(
                "allow_partial_completion", default_.allow_partial_completion);
        }
        catch (const json::out_of_range& e) { handle_not_found(e); } catch (const
            std::exception& e) { std::cerr << "Uknown error " << e.what() << "\n"; }
        return value_;
    }

    template <typename T>
    void ManagedVar<T>::set(const T& value)
    {
        value_ = value;
        save();
    }

    template <typename T>
    void ManagedVar<T>::save()
    {
        std::lock_guard<std::mutex> guard(save_mutex);
        json& curr = walk_json(get_data(), true);

        curr[path_.back()] = value_;
        on_change_();
    }

    template <>
    void ManagedVar<bots::drops::CrateManagerConfig>::save()
    {
        std::lock_guard<std::mutex> guard(save_mutex);
        json& curr = walk_json(get_data(), true);
        const std::string k = path_.back();

        curr[k]["prefix"] = value_.prefix;
        curr[k]["grouped_crates_raw"] = value_.grouped_crates_raw;
        curr[k]["interval"] = value_.interval;
        curr[k]["render_for"] = value_.render_for;
        curr[k]["uses_teleporters"] = value_.uses_teleporters;
        curr[k]["overrule_reroll_mode"] = value_.overrule_reroll_mode;
        curr[k]["allow_partial_completion"] = value_.allow_partial_completion;

        on_change_();
    }

    template <typename T>
    json& ManagedVar<T>::walk_json(json& data, const bool create_if_not_exist) const
    {
        json* curr = &data;
        for (int i = 0; i < path_.size() - 1; i++) {
            if (create_if_not_exist) { curr = &(*curr)[path_[i]]; }
            else { curr = &curr->at(path_[i]); }
        }
        return *curr;
    }

    template <typename T>
    void ManagedVar<T>::handle_not_found(const json::out_of_range& e)
    {
        if (has_inserted_default_) { throw e; }
        std::cout << std::format(
            "[!] ManagedVar '{}' was not found, inserting default value.\n",
            path_.back());
        set(default_);
    }

    template struct ManagedVar<std::vector<int>>;
    template struct ManagedVar<std::vector<char>>;
    template struct ManagedVar<std::array<char, 256>>;
    template struct ManagedVar<std::vector<std::string>>;
    template struct ManagedVar<std::vector<const char*>>;
    template struct ManagedVar<bots::drops::CrateManagerConfig>;

    template struct ManagedVar<std::string>;
    template struct ManagedVar<const char*>;
    template struct ManagedVar<bool>;
    template struct ManagedVar<int>;
    template struct ManagedVar<std::filesystem::path>;
}
