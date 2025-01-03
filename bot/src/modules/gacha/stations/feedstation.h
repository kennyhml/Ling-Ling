#pragma once
#include "core/teleportstation.h"


namespace lingling
{
    class seedstation final : public teleport_station
    {
    public:
        explicit seedstation(std::string t_name);

        void complete() override;

    private:
        asa::dino_entity iguanodon_;
    };



}
