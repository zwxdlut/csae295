#ifndef __SOCKETLIB__H__
#define __SOCKETLIB__H__

#include <stdint.h>
#include <stdbool.h>

#include <sys/types.h>
#include <time.h>
#include <signal.h>

#include <thread>

namespace socketlib
{
/**
 * Socket connect state.
 */
enum ConnectState
{
    CLOSED = 0,
    CONNECTED = 1, 
    LOST = 2
};

typedef void (*connect_state_callback)(const ConnectState _state, void *_param);
    
/**
 * Socket client.
 */
class Client
{
public:
    ~Client();

    void set_connect_state_callback(connect_state_callback _callback, void *_param);

    int32_t open(const char *_addr, const uint32_t _port, const bool _block = true);

    ssize_t recv(void *_buf, size_t _size);

    ssize_t send(const void *_buf, size_t _size);

    int32_t close();

private:
    void listen();

    static constexpr const char *TAG = "socketlib::Client";

    char addr_port_[64]="";
    int32_t sockfd_;
    ConnectState state_ = CLOSED;
    // bool done_ = true;
    // std::thread thread_;
    timer_t timer_;
    connect_state_callback callback_ = nullptr;
    void *param_ = nullptr;
};
}

#endif // __SOCKETLIB__H__