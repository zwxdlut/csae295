#ifndef __CSAE295_PACKER_H__
#define __CSAE295_PACKER_H__

#include <string>
#include <vector>
#include <memory>
#include <type_traits>

#include "protocol/message/message.h"
#include "protocol/message/veh2cloud_inh.h"
#include "protocol/message/veh2cloud_state.h"

namespace csae295
{
/**
 * Message packer.
 */
class Packer
{
public:
    /**
     * Packer handler.
     */
    class Handler
    {
    public:
        virtual ~Handler() {}

        virtual void on_message(const MessageHeader &_msg) {}

        virtual void on_message(const Veh2CloudInh &_msg) {}
    
        virtual void on_message(const Cloud2VehInhRes &_msg) {}

        virtual void on_message(const Veh2CloudState &_msg) {}

        virtual void on_message(const Veh2CloudState2 &_msg) {}
    };

    template<typename T , typename std::enable_if<std::is_base_of<MessageHeader, T>::value>::type* = nullptr>
    static std::shared_ptr<MessageBuffer> pack(const T &_msg)
    {
        size_t size = _msg.header_length() + _msg.data_length();
        std::vector<uint8_t> v;

        // pack message
        uint8_t buf[size];
        size = _msg.to_bytes(buf, size);
        v.insert(v.end(), buf, buf + size);

        // copy memory
        size = v.size();
        std::shared_ptr<MessageBuffer> p(
            (MessageBuffer*)new uint8_t[size + sizeof(MessageBuffer)], 
            MessageBuffer::deleter<MessageBuffer>);
        p->size = size;
        memcpy(p->data, v.data(), size);

        return p;
    }

    static std::shared_ptr<MessageHeader> unpack(const void *_buf, const size_t _size, Handler *_handler = nullptr);

private:
    static constexpr const char *TAG = "csae295::Packer";
};
} // namespace csae295

#endif // __CSAE295_PACKER_H__