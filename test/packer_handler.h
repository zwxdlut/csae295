#ifndef __PACKER_HANDLER_H__
#define __PACKER_HANDLER_H__

#include "protocol/packer.h"

using namespace csae295;

class PackerHandler : public Packer::Handler
{
public:
    void on_message(const MessageHeader &_msg) override
    {
        std::cout << _msg << std::endl;
    }

    void on_message(const Veh2CloudInh &_msg) override
    {
        std::cout << _msg << std::endl;
    }

    void on_message(const Cloud2VehInhRes &_msg) override
    {
        std::cout << _msg << std::endl;
    }

    void on_message(const Veh2CloudState &_msg) override
    {
        std::cout << _msg << std::endl;
    }
};

#endif // __PACKER_HANDLER_H__
