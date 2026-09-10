#include "protocol/packer.h"

#include "util/log.h"

namespace csae295
{
std::shared_ptr<MessageHeader> Packer::unpack(const void *_buf, const size_t _size, Handler *_handler)
{
    if (nullptr == _buf || 0 == _size)
    {
        LOGE(TAG, "buffer is null or _size is 0!\n");
        return nullptr;
    }

    uint8_t *buf = (uint8_t*)_buf;
    if (0xF2 != buf[0])
    {   
        LOGE(TAG, "No identifier!\n");
        return nullptr;
    }
    
    // unpack message
    uint8_t data_type = *(uint8_t*)(buf + 5);

    switch (data_type)
    {
        case HEARTBEAT:
        {
            auto msg = std::make_shared<MessageHeader>(buf, _size);
            if (nullptr != _handler)
            {
                _handler->on_message(*msg);
            }
            return msg;
        }

        case HEARTBEAT_RES:
        {
            auto msg = std::make_shared<MessageHeader>(buf, _size);
            if (nullptr != _handler)
            {
                _handler->on_message(*msg);
            }
            return msg;
        }

        case VEH2CLOUD_INH:
        {
            auto msg = std::make_shared<Veh2CloudInh>(buf, _size);
            if (nullptr != _handler)
            {
                _handler->on_message(*msg);
            }
            return msg;
        }

        case CLOUD2VEH_INH_RES:
        {
            auto msg = std::make_shared<Cloud2VehInhRes>(buf, _size);
            if (nullptr != _handler)
            {
                _handler->on_message(*msg);
            }
            return msg;
        }

        case VEH2CLOUD_STATE:
        {
            uint8_t version = *(uint8_t*)(buf + 6);

            if (0x01 == version)
            {
                auto msg = std::make_shared<Veh2CloudState>(buf, _size);
                if (nullptr != _handler)
                {
                    _handler->on_message(*msg);
                }
                return msg;
            }
            else if (0x02 == version)
            {
                auto msg = std::make_shared<Veh2CloudState2>(buf, _size);
                if (nullptr != _handler)
                {
                    _handler->on_message(*msg);
                }
                return msg;
            }
        }

        default:
            break;
    }

    return nullptr;
}
} // namespace csae295


