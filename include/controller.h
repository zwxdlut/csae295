#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "protocol/packer.h"
#include "util/timer.h"
#include "util/block_queue.h"
#include "util/socketlib.h"

#define UP_CHANNEL      0   // data up channel
#define DOWN_CHANNEL    1   // instruction down channel

using namespace csae295;

/**
 * Controller.
 */
class Controller : public Packer::Handler
{
public:
    /**
     * Controller callback.
     */
    class Callback
    {
    public:
        virtual ~Callback() {}

        virtual void on_up_connect_state(const socketlib::ConnectState _state) {};

        virtual void on_down_connect_state(const socketlib::ConnectState _state) {};

        virtual void on_message(const MessageHeader &_msg) {};

        virtual void on_message(const Veh2CloudInh &_msg) {};

        virtual void on_message(const Cloud2VehInhRes &_msg) {};

        virtual void on_message(const Veh2CloudState &_msg) {};

    };

    ~Controller();

    void set_callback(Callback *_callback);

    void start(
        const std::string &_up_addr, const uint32_t _up_port, 
        const std::string &_down_addr, const uint32_t _down_port);
    
    void start(const std::string &_addr, const uint32_t _port, const uint8_t _ch);
    
    void stop();

    void stop(const uint8_t _ch);

    // send methods

    void send(const std::shared_ptr<MessageBuffer> _buf, const uint8_t _ch);

    void batch(const std::vector<std::shared_ptr<MessageBuffer>> &_bufs, const uint8_t _ch);

    template<typename T , typename std::enable_if<std::is_base_of<MessageHeader, T>::value>::type* = nullptr>
    void send(const T &_msg, const uint8_t _ch)
    {
        auto buf = Packer::pack(_msg);
        send(buf, _ch);
    }

    // handler methods

    void on_message(const MessageHeader &_msg) override;

    void on_message(const Veh2CloudInh &_msg) override;

    void on_message(const Cloud2VehInhRes &_msg) override;

    void on_message(const Veh2CloudState &_msg) override;

private:
    // up channel

    void up_sock_recv_thread();

    void up_sock_send_thread();

    void on_up_connect_state(const socketlib::ConnectState _state);

    // down channel

    void down_sock_recv_thread();

    void down_sock_send_thread();

    void on_down_connect_state(const socketlib::ConnectState _state);

    static constexpr const char *TAG = "Controller";

    Callback *callback_ = nullptr;

    // up channel

    bool up_stopped_ = true;
    std::string up_addr_;
    uint32_t up_port_;
    socketlib::Client up_sock_;
    std::thread  up_recv_thread_;
    std::thread  up_send_thread_;
    BlockQueue<std::shared_ptr<MessageBuffer>> up_send_queue_;

    // down channel

    bool down_stopped_ = true;
    std::string down_addr_;
    uint32_t down_port_;
    socketlib::Client down_sock_;
    std::thread down_recv_thread_;
    std::thread down_send_thread_;
    BlockQueue<std::shared_ptr<MessageBuffer>> down_send_queue_;

    Timer heartbeat_timer_;
};

#endif // __CONTROLLER_H__
