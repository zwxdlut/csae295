#include <stdio.h>

#include "controller.h"
#include "util/util.h"

#define UP_SERVER_ADDRESS   "60.16.59.129"
#define UP_SERVER_PORT      50011
#define DOWN_SERVER_ADDRESS "60.16.59.129"
#define DOWN_SERVER_PORT    50012

class ControllerCallback: public Controller::Callback
{
public:
    ControllerCallback(Controller &_controller): controller_(_controller), inh_count_(0) {}

    ~ControllerCallback() 
    {
        state_timer_.stop();
        inh_timer_.stop();
    }

    void on_up_connect_state(const socketlib::ConnectState _state) override
    {
        LOGD(TAG, "state %d\n", _state);

        if (socketlib::ConnectState::CONNECTED == _state)
        {
            LOGD(TAG, "The connection is established.\n");

            // VEH2CLOUD_INH
            inh_timer_.start(
                1000, [](void *_param)
                {
                    ControllerCallback *self = static_cast<ControllerCallback *>(_param);

                    if (3 <= self->inh_count_)
                    {
                        LOGE(TAG, "connection exception, VEH2CLOUD_INH count %zu!\n", self->inh_count_);
                        self->inh_timer_.stop();
                        self->inh_count_ = 0;
                        self->state_timer_.stop();
                        return;
                    }

                    Veh2CloudInh msg(
                        0x01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", "sw_v1.0", 
                        "hw_v1.0", "ad_v1.0", COMM_TYPE_4G, 15, TIME_SYNC_GNSS, GNSS_TYPE_GCJ02, "VEH2CLOUD_INH");
                    self->controller_.send(msg, UP_CHANNEL);
                    self->inh_count_++;
                }, this);

            // VEH2CLOUD_STATE
            state_timer_.start(
                3000, [](void *_param)
                {
                    ControllerCallback *self = static_cast<ControllerCallback *>(_param);
                    auto timestamp = get_utc_timestamp_ms();
                    Veh2CloudState msg(
                        0x01, timestamp, CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", std::vector<uint8_t>{1}, 
                        timestamp, 4000, Position(90, 90, 700), 100000, 31, 200000, 4100, 500, 400, 300, 200, 500, 
                        3000, 50000, 1, 500, 20000, 1000, 1, Position2D(80, 80), std::vector<Position2D>());
                    self->controller_.send(msg, UP_CHANNEL);
                }, this);
        }
        else
        {
            LOGE(TAG, "The connection is lost!\n");

            state_timer_.stop();
            inh_timer_.stop();
            inh_count_ = 0;
        }
    }

    void on_down_connect_state(const socketlib::ConnectState _state) override
    {
        LOGD(TAG, "state %d\n", _state);

        if (socketlib::ConnectState::CONNECTED == _state)
        {
            LOGD(TAG, "The connection is established.\n");
        }
        else
        {
            LOGE(TAG, "The connection is lost.\n");
        }
    }

    void on_message(const Cloud2VehInhRes &_msg) override
    {
        LOGD(TAG, "Cloud2VehInhRes\n");
        inh_timer_.stop();
        inh_count_ = 0;
    }

private:
    static constexpr const char *TAG = "ControllerCallback";

    Controller &controller_;
    Timer state_timer_;
    Timer inh_timer_;
    size_t inh_count_ = 0;
};

int main(int argc, char *argv[])
{
    Controller controller;
    ControllerCallback callback(controller);

    controller.set_callback(&callback);
    controller.start(UP_SERVER_ADDRESS, UP_SERVER_PORT, DOWN_SERVER_ADDRESS, DOWN_SERVER_PORT);

    while (1) {}
    
    // never reach
    controller.stop();

    return 0;
}