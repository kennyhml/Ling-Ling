#pragma once
#include <exception>
#include <string>

namespace llpp::bots::farm
{
    class NotOnTeleporterError final : public std::exception
    {
    public:
        explicit NotOnTeleporterError(std::string t_fell_of_where)
            : fell_of_where_(std::move(t_fell_of_where)) {};

        [[nodiscard]] char const* what() const override { return fell_of_where_.c_str(); }

    private:
        std::string fell_of_where_;
    };
}
