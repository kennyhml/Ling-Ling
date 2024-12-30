#pragma once
#include <exception>
#include <string>

namespace lingling
{
    struct lingling_error : std::exception
    {
    public:
        explicit lingling_error(std::string t_what) : info_(std::move(t_what)) {}

        [[nodiscard]] char const* what() const override { return info_.c_str(); }

    private:
        std::string info_;
    };
}
