#include "setup.h"
#include "tribelogrole.h"
namespace lingling::discord {

    void listener()
    {
        add_setup_command_create_listener(triberole);
    }
}
