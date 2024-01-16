#pragma once
#include "../../external/keyauth/skStr.h"
#include "../../external/keyauth/auth.hpp"

namespace llpp::auth
{
    using namespace KeyAuth;

    class ConnectionError : public std::exception
    {
    public:
        ConnectionError(const std::string& t_message) : message_(t_message) {}

        const char* what() const noexcept override { return message_.c_str(); }

    private:
        std::string message_;
    };


    void login();
}
