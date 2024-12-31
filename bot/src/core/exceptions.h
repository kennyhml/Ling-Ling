#pragma once
#include <exception>
#include <string>

namespace lingling
{
    /**
     * @brief Base ling ling exception.
     */
    struct lingling_error : std::exception
    {
    public:
        explicit lingling_error(std::string t_what) : info_(std::move(t_what)) {}

        [[nodiscard]] char const* what() const override { return info_.c_str(); }

    private:
        std::string info_;
    };

    /**
     * @brief Thrown when the startup has failed, the error is considered fatal to the application.
     */
    struct startup_failed final : lingling_error
    {
    public:
        explicit startup_failed(const std::string& t_why)
            : lingling_error(std::format("Startup failed - {}!", t_why)) {}
    };
}
