#include "controller.h"

#include "util/log.h"
#include "util/util.h"

Controller::~Controller()
{
    stop();
}

void Controller::set_callback(Callback *_callback)
{
    callback_ = _callback;
}

void Controller::start(
    const std::string &_up_addr, const uint32_t _up_port, 
    const std::string &_down_addr, const uint32_t _down_port)
{  
    start(_up_addr, _up_port, UP_CHANNEL);
    start(_down_addr, _down_port, DOWN_CHANNEL);
}

void Controller::start(const std::string &_addr, const uint32_t _port, const uint8_t _ch)
{
    if (UP_CHANNEL == _ch)
    {
        up_addr_ = _addr;
        up_port_ = _port;
        up_stopped_ = false;

        // create socket and connect it to server
        up_sock_.open(up_addr_.c_str(), up_port_);
        
        // receive thread
        up_recv_thread_ = std::thread([this]()
        {
            this->up_sock_recv_thread();
        });

        // send thread
        up_send_thread_ = std::thread([this]()
        {
            this->up_sock_send_thread();
        });

        // up callback
        up_sock_.set_connect_state_callback([](const socketlib::ConnectState _state, void *_param)
        {
            Controller *self = static_cast<Controller *>(_param);
            self->on_up_connect_state(_state);
        }, this);
    }
    else if (DOWN_CHANNEL == _ch)
    {
        down_addr_ = _addr;
        down_port_ = _port;
        down_stopped_ = false;

        // create socket and connect it to server
        down_sock_.open(down_addr_.c_str(), down_port_);
        
        // receive thread
        down_recv_thread_ = std::thread([this]()
        {
            this->down_sock_recv_thread();
        });

        // send thread
        down_send_thread_ = std::thread([this]()
        {
            this->down_sock_send_thread();
        });

        // down callback
        down_sock_.set_connect_state_callback([](const socketlib::ConnectState _state, void *_param)
        {
            Controller *self = static_cast<Controller *>(_param);
            self->on_down_connect_state(_state);
        }, this);
    }
}

void Controller::stop()
{
    stop(UP_CHANNEL);
    stop(DOWN_CHANNEL);
}

void Controller::stop(const uint8_t _ch)
{
    if (UP_CHANNEL == _ch)
    {
        if (up_stopped_)
        {
            return;
        }

        up_stopped_ = true;
        up_sock_.close();
        up_recv_thread_.join();
        up_send_queue_.notify();
        up_send_thread_.join();
    }
    else if (DOWN_CHANNEL == _ch)
    {
        if (down_stopped_)
        {
            return;
        }

        heartbeat_timer_.stop();
        down_stopped_ = true;
        down_sock_.close();
        down_recv_thread_.join();
        down_send_queue_.notify();
        down_send_thread_.join();
    }
}

//  send methods

void Controller::send(const std::shared_ptr<MessageBuffer> _buf, const uint8_t _ch)
{
    if (UP_CHANNEL == _ch)
    {
        up_send_queue_.put(_buf); 
    }
    else if (DOWN_CHANNEL == _ch)
    {
        down_send_queue_.put(_buf);
    }
}

void Controller::batch(const std::vector<std::shared_ptr<MessageBuffer>> &_bufs, const uint8_t _ch)
{
    for (const auto &buf : _bufs)
    {
        send(buf, _ch);
    }
}

//  handler methods

void Controller::on_message(const MessageHeader &_msg)
{
    std::cout << std::endl << _msg;

    switch (_msg.data_type)
    {
    case HEARTBEAT_RES:
        // TODO: 
        break;
    
    default:
        break;
    }

    if (nullptr != callback_)
    {
        callback_->on_message(_msg);
    }
}

void Controller::on_message(const Veh2CloudInh &_msg)
{
    std::cout << std::endl << _msg;

    if (nullptr != callback_)
    {
        callback_->on_message(_msg);
    }
}

void Controller::on_message(const Cloud2VehInhRes &_msg)
{
    std::cout << std::endl << _msg;

    if (nullptr != callback_)
    {
        callback_->on_message(_msg);
    }
}

void Controller::on_message(const Veh2CloudState &_msg)
{
    std::cout << std::endl << _msg;

    if (nullptr != callback_)
    {
        callback_->on_message(_msg);
    }
}

// private

// up channel

void Controller::up_sock_recv_thread()
{
    uint8_t buf[4096] = {0};
    ssize_t size = 0;

    while (!up_stopped_)
    {
        size = up_sock_.recv(buf, sizeof(buf));

        if (0 == size)
        {
            LOGE(TAG, "remote shutdown, exit!\n");
            return;
        }
        else if (0 > size)
        {
            if (EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno)
            {
                LOGW(TAG, "continue receive!\n");
                continue;
            }
            else
            {
                LOGE(TAG, "receive error, exit!\n");
                return;
            }
        }
        
        printf("\n");
        print_buffer("SOCK-RX(UP)", 0, buf, size);
        Packer::unpack(buf, size, this);
    }
}

void Controller::up_sock_send_thread()
{
    while (!up_stopped_)
    {	
        auto p = up_send_queue_.take();

        if (nullptr == p)
        {
            continue;
        }
        
        printf("\n");
        print_buffer("SOCK-TX(UP)", 0, p->data, p->size);
        up_sock_.send(p->data, p->size);
        up_send_queue_.pull();
    }
}

void Controller::on_up_connect_state(const socketlib::ConnectState _state)
{
    LOGD(TAG, "state %d\n", _state);

    if (socketlib::ConnectState::CONNECTED == _state)
    {
        LOGD(TAG, "The connection is established.\n");
    }
    else
    {
        LOGW(TAG, "The connection is lost, and a reconnection is scheduled in 5 seconds.\n");
        stop(UP_CHANNEL);

        std::thread t([&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            start(up_addr_, up_port_, UP_CHANNEL);
        });
        t.detach();
    }

    if (nullptr != callback_)
    {
        callback_->on_up_connect_state(_state);
    }
}

// down channel

void Controller::down_sock_recv_thread()
{
    uint8_t buf[4096] = {0};
    ssize_t size = 0;

    while (!down_stopped_)
    {
        size = down_sock_.recv(buf, sizeof(buf));

        if (0 == size)
        {
            LOGE(TAG, "remote shutdown, exit!\n");
            return;
        }
        else if (0 > size)
        {
            if (EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno)
            {
                LOGW(TAG, "continue receive!\n");
                continue;
            }
            else
            {
                LOGE(TAG, "receive error, exit!\n");
                return;
            }
        }
        
        printf("\n");
        print_buffer("SOCK-RX(DOWN)", 0, buf, size);
        Packer::unpack(buf, size, this);
    }
}

void Controller::down_sock_send_thread()
{
    while (!down_stopped_)
    {	
        auto p = down_send_queue_.take();

        if (nullptr == p)
        {
            continue;
        }
        
        printf("\n");
        print_buffer("SOCK-TX(DOWN)",0, p->data, p->size);
        down_sock_.send(p->data, p->size);
        down_send_queue_.pull();
    }
}

void Controller::on_down_connect_state(const socketlib::ConnectState _state)
{
    LOGD(TAG, "state %d\n", _state);

    if (socketlib::ConnectState::CONNECTED == _state)
    {
        LOGD(TAG, "The connection is established.\n");

        heartbeat_timer_.start(10000, [](void *_param)
        {
            Controller *self = static_cast<Controller *>(_param);
            MessageHeader msg(0, HEARTBEAT, VERSION_01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE);
            self->send(msg, DOWN_CHANNEL);
        }, this, true);
    }
    else
    {
        LOGW(TAG, "The connection is lost, and a reconnection is scheduled in 5 seconds.\n");
        stop(DOWN_CHANNEL);

        std::thread t([&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            start(down_addr_, down_port_, DOWN_CHANNEL);
        });
        t.detach();
    }

    if (nullptr != callback_)
    {
        callback_->on_down_connect_state(_state);
    }
}
