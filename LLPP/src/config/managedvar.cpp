#include "managedvar.h"


namespace llpp::config
{
    template <typename T>
    T ManagedVar<T>::get()
    {
        json& curr = walk_json(get_data());
        value_ = curr.at(path_.back()).get<T>();
        return value_;
    }

    template <>
    const char* ManagedVar<const char*>::get()
    {
        json& curr = walk_json(get_data());
        value_ = _strdup(curr.at(path_.back()).get<std::string>().c_str());
        return value_;
    }

    template <>
    std::vector<const char*> ManagedVar<std::vector<const char*>>::get()
    {
        json& curr = walk_json(get_data());
        const auto& array = curr.at(path_.back());

        value_.clear();
        try {
            for (const auto& element : array) {
                std::cout << element << "\n";
                value_.emplace_back(_strdup(element.get<std::string>().c_str()));
            }
            return value_;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << '\n';
            return {};
        }
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

    template struct ManagedVar<std::vector<int>>;
    template struct ManagedVar<std::vector<char>>;
    template struct ManagedVar<std::array<char, 256>>;
    template struct ManagedVar<std::vector<std::string>>;
    template struct ManagedVar<std::vector<const char*>>;


    template struct ManagedVar<std::string>;
    template struct ManagedVar<const char*>;
    template struct ManagedVar<bool>;
    template struct ManagedVar<std::filesystem::path>;
}
