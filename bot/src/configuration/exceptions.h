#pragma once
#include <format>

#include "core/exceptions.h"

namespace lingling
{
    struct duplicate_category_callback final : lingling_error
    {
    public:
        explicit duplicate_category_callback(const std::string& t_cat)
            : lingling_error(
                std::format("Category '{}' already has a validation callback!", t_cat)
            ) {}
    };
}
