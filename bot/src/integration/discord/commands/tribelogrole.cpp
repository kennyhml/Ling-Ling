#include "setup.h"
#include "tribelogrole.h"
namespace lingling::discord {

    void listener()
    {
        std::cout << "start" << "\n";
        add_setup_command_create_listener(triberole);
        std::cout << "end" << "\n";
    }
    void start_listener()
    {
        listener();
    }
}