#ifndef __TEST_H__
#define __TEST_H__

#include <chrono>
#include <iostream>

#include "protocol/packer.h"
#include "util/util.h"
#include "packer_handler.h"

#define SPLIT_LINE    (std::string(60, '='))

using namespace csae295;

class Test
{
public:
    template<typename T, typename std::enable_if<std::is_base_of<MessageHeader, T>::value>::type* = nullptr>
    static void run(const T &_t) 
    {
        std::cout << std::endl << SPLIT_LINE << std::endl << std::endl << _t << std::endl;
        
        // pack
        auto buf = Packer::pack(_t);
        print_buffer("CSAE295", _t.data_type, buf->data, buf->size);
        std::cout << std::endl;

        // unpack
        PackerHandler handler;
        Packer::unpack(buf->data, buf->size, &handler);
        std::cout << SPLIT_LINE << std::endl;
    }

    template<typename T, typename std::enable_if<std::is_base_of<MessageHeader, T>::value>::type* = nullptr>
    static void test() 
    {
        LOGE("Test", "T isn't specified!\n");
    }
};

template<>
inline void Test::test<MessageHeader>() 
{
    MessageHeader msg(0, HEARTBEAT, VERSION_01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE);
    run(msg);

    MessageHeader msg2(0, HEARTBEAT_RES, VERSION_01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE);
    run(msg2);
}

template<>
inline void Test::test<Veh2CloudInh>() 
{
    Veh2CloudInh msg(
        VERSION_01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", "sw_v1.0", 
        "hw_v1.0", "ad_v1.0", COMM_TYPE_4G, 15, TIME_SYNC_GNSS, GNSS_TYPE_GCJ02, "VEH2CLOUD_INH");
    run(msg);
}

template<>
inline void Test::test<Cloud2VehInhRes>() 
{
    Cloud2VehInhRes msg(
        VERSION_01, get_utc_timestamp_ms(), CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", CLOUD2VEH_INH_RES_COMFIRM);
    run(msg);
}

template<>
inline void Test::test<Veh2CloudState>() 
{
    auto timestamp = get_utc_timestamp_ms();
    Veh2CloudState msg(
        VERSION_01,timestamp, CTRL_PRIORITY_7 | CTRL_ENCRYPTION_NONE, "Q1001", std::vector<uint8_t>{1}, timestamp, 
        4000, Position(90, 90, 700), 100000, 31, 200000, 4100, 500, 400, 300, 200, 500, 3000, 50000, 1, 500, 20000, 
        1000, 1, Position2D(80, 80), std::vector<Position2D>());
    run(msg);
}

#endif // __TEST_H__
