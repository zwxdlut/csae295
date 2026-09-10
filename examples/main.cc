#include <stdio.h>

#include "controller.h"
#include "util/util.h"

#define UP_SERVER_ADDRESS   "60.16.59.129"
#define UP_SERVER_PORT      50011
#define DOWN_SERVER_ADDRESS "60.16.59.129"
#define DOWN_SERVER_PORT    50012

Controller g_controller;
Timer g_state_timer;
Timer g_inh_timer;
size_t g_inh_count = 0;

class ControllerCallback: public Controller::Callback
{
public:
    void on_up_connect_state(const socketlib::ConnectState _state)
    {
        LOGD(TAG, "state %d\n", _state);

        if (socketlib::ConnectState::CONNECTED == _state)
        {
            LOGD(TAG, "The connection is established.\n");

            // VEH2CLOUD_INH
            g_inh_timer.start(1000, [](void *_param)
            {
                if (3 <= g_inh_count)
                {
                    LOGE(TAG, "Connection exception! VEH2CLOUD_INH count %zu\n", g_inh_count);
                    g_inh_timer.stop();
                    g_inh_count = 0;
                    g_state_timer.stop();
                    // g_controller.stop(UP_CHANNEL);
                    // g_controller.start(UP_SERVER_ADDRESS, UP_SERVER_PORT, UP_CHANNEL);
                    return;
                }

                Veh2CloudInh msg(
                    0x01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", "sw_v1.0", 
                    "hw_v1.0", "ad_v1.0", COMM_TYPE_4G, 15, TIME_SYNC_GNSS, GNSS_TYPE_GCJ02, "VEH2CLOUD_INH");
                g_controller.send(msg, UP_CHANNEL);
                g_inh_count++;
            }, nullptr);

            // VEH2CLOUD_STATE
            g_state_timer.start(3000, [](void *_param)
            {
                auto timestamp = get_utc_timestamp_ms();
                Veh2CloudState msg(
                    0x01, timestamp, CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", std::vector<uint8_t>{1}, 
                    timestamp, 4000, Position(90, 90, 700), 100000, 31, 200000, 4100, 500, 400, 300, 200, 500, 
                    3000, 50000, 1, 500, 20000, 1000, 1, Position2D(80, 80), std::vector<Position2D>());
                g_controller.send(msg, UP_CHANNEL);
            }, nullptr);
        }
        else
        {
            LOGW(TAG, "The connection is lost.\n");

            g_inh_timer.stop();
            g_inh_count = 0;
            g_state_timer.stop();
        }
    }

    void on_down_connect_state(const socketlib::ConnectState _state)
    {
        LOGD(TAG, "state %d\n", _state);

        if (socketlib::ConnectState::CONNECTED == _state)
        {
            LOGD(TAG, "The connection is established.\n");
        }
        else
        {
            LOGW(TAG, "The connection is lost.\n");
        }
    }

    void on_message(const Cloud2VehInhRes &_msg) override
    {
        LOGD(TAG, "Cloud2VehInhRes\n");
        g_inh_timer.stop();
    }

private:
    static constexpr const char *TAG = "ControllerCallback";
};

int main(int argc, char *argv[])
{
    ControllerCallback callback;

    g_controller.set_callback(&callback);
    g_controller.start(UP_SERVER_ADDRESS, UP_SERVER_PORT, DOWN_SERVER_ADDRESS, DOWN_SERVER_PORT);

    while (1) {}
    
    // never reach
    g_state_timer.stop();
    g_inh_timer.stop();
    g_inh_count = 0;
    g_controller.stop();

    return 0;
}