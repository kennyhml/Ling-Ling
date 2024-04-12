#include "managedcommand.h"

namespace llpp::discord
{
    ManagedCommand::ManagedCommand(const std::string& t_name,
                                   const std::string& t_description,
                                   const dpp::snowflake t_application_id,
                                   event_callback_t t_callback,
                                   const bool t_is_static)
        : slashcommand(t_name, t_description, t_application_id), is_static_(t_is_static),
          callback_(std::move(t_callback)) {}
}
