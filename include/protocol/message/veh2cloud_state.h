#ifndef __CSAE295_VEH2CLOUD_STATE_H__
#define __CSAE295_VEH2CLOUD_STATE_H__

#include <algorithm>

#include "protocol/message/message.h"

/**
 * @name Brake flag
 * @{
 */
#define BREAK_FLAG_UP       0
#define BREAK_FLAG_DOWN     1
#define BREAK_FLAG_DEFAULT  0xFF
/** @} */ // Brake flag

/**
 * @name Drive mode
 * @{
 */
#define DRIVE_MODE_MANUAL           1
#define DRIVE_MODE_AUTO             2
#define DRIVE_MODE_CLOUD_MANUAL     3 
#define DRIVE_MODE_CLOUD_AUTO       4 
#define DRIVE_MODE_NO_DIRVER_MANUAL 5 
#define DRIVE_MODE_DETACH           6
#define DRIVE_MODE_NONE             7
#define DRIVE_MODE_REMOTE           8 
#define DRIVE_MODE_NOT_DEFINED      9 
#define DRIVE_MODE_DEFAULT          0xFF
/** @} */ // Drive mode

/**
 * @name Light state
 * @{
 */
#define LIGHT_STATE_NEAR            (1)
#define LIGHT_STATE_FAR             (1 << 1)
#define LIGHT_STATE_LEFT            (1 << 2)
#define LIGHT_STATE_RIGTH           (1 << 3)
#define LIGHT_STATE_DOUBLE          (1 << 4)
#define LIGHT_STATE_AUTO_CONTROL    (1 << 5)
#define LIGHT_STATE_DAY_DRIVING     (1 << 6)
#define LIGHT_STATE_FRONT_FOG       (1 << 7)
#define LIGHT_STATE_BACK_FOG        (1 << 8)
#define LIGHT_STATE_PARKING         (1 << 9)
#define LIGHT_STATE_POSITION        (1 << 10)
#define LIGHT_STATE_REVERSING       (1 << 11)
#define LIGHT_STATE_BRAKE           (1 << 12)
#define LIGHT_STATE_INVALID         (1 << 15)
/** @} */ // Light state

namespace csae295
{
#pragma pack(1)

struct Position
{
    Position() {}
    Position(const uint32_t _longitude, const uint32_t _latitude, const uint32_t _elevation):
        longitude(_longitude), latitude(_latitude), elevation(_elevation) {}

    uint32_t longitude;
    uint32_t latitude;
    uint32_t elevation;
};

struct Position2D
{
    Position2D() {}
    Position2D(const uint32_t _longitude, const uint32_t _latitude):
        longitude(_longitude), latitude(_latitude) {}

    uint32_t longitude;
    uint32_t latitude;
};

/**
 * VEH2CLOUD_STATE.
 */
struct Veh2CloudState : public MessageHeader
{
    Veh2CloudState() {}

    Veh2CloudState(
        const uint8_t _version,
        const uint64_t _timestamp,
        const uint8_t _ctrl,
        const std::string &_vehicle_id,
        const std::vector<uint8_t> &_message_id,
        const uint64_t _gnss_timestamp,
        const uint16_t _gnss_velocity,
        const Position &_position,
        const uint32_t _heading,
        const uint8_t  _gear,
        const uint32_t _steering_angle,
        const uint16_t _velocity,
        const uint16_t _acc_lon,
        const uint16_t _acc_lat,
        const uint16_t _acc_ver,
        const uint16_t _yaw_rate,
        const uint16_t _accel_pos,
        const uint16_t _engine_speed,
        const uint32_t _engine_torque,
        const uint8_t  _break_flag,
        const uint16_t _break_pos,
        const uint16_t _break_pressure,
        const uint16_t _fuel_consume,
        const uint8_t _drive_mode,
        const Position2D &_dest_location,
        const std::vector<Position2D> &_pass_pos): 
            MessageHeader(
                sizeof(Veh2CloudState) - sizeof(MessageHeader) - sizeof(pass_pos) + _pass_pos.size(), 
                VEH2CLOUD_STATE, 
                _version, 
                _timestamp, 
                _ctrl), 
            gnss_timestamp(_gnss_timestamp), 
            gnss_velocity(_gnss_velocity), 
            position(_position), 
            heading(_heading), 
            gear(_gear), 
            steering_angle(_steering_angle), 
            velocity(_velocity), 
            acc_lon(_acc_lon), 
            acc_lat(_acc_lat), 
            acc_ver(_acc_ver), 
            yaw_rate(_yaw_rate), 
            accel_pos(_accel_pos), 
            engine_speed(_engine_speed), 
            engine_torque(_engine_torque), 
            break_flag(_break_flag), 
            break_pos(_break_pos), 
            break_pressure(_break_pressure), 
            fuel_consume(_fuel_consume), 
            drive_mode(_drive_mode), 
            dest_location(_dest_location), 
            pass_pos_num(_pass_pos.size()), 
            pass_pos(_pass_pos)
    {
        size_t n = sizeof(vehicle_id) <= _vehicle_id.length() ? sizeof(vehicle_id) : _vehicle_id.length();
        strncpy(vehicle_id, _vehicle_id.c_str(), n);

        n = sizeof(message_id) <= _message_id.size() ? sizeof(message_id) : _message_id.size();
        memcpy(message_id, _message_id.data(), n);
    }

    Veh2CloudState(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        from_bytes(_buf, _size, _big_endian);
    }

    size_t length() const
    {
        return sizeof(Veh2CloudState) - sizeof(pass_pos) + pass_pos.size();
    }

    size_t from_bytes(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        if (nullptr == _buf)
        {
            LOGE(TAG, "Buffer is null!\n");
            return 0;
        }

        size_t offset = MessageHeader::from_bytes(_buf, _size, _big_endian);
        size_t len = length() - MessageHeader::length();
        if (_size - offset < len)
        {
            LOGE(TAG, "Buffer size %ld is not enough, offset %ld, data length %ld!\n", 
                _size, offset, len);
            return offset;
        }

        uint8_t *buf = (uint8_t*)_buf;

        memcpy(vehicle_id, buf + offset, sizeof(vehicle_id));
        offset += sizeof(vehicle_id);

        std::vector<uint8_t> v(buf + offset, buf + offset + sizeof(message_id));
        if (_big_endian)
        {
            std::reverse(v.begin(), v.end());
        }
        memcpy(message_id, v.data(), sizeof(message_id));
        offset += sizeof(message_id);

        gnss_timestamp = _big_endian ? __builtin_bswap64(*(uint64_t*)(buf + offset)) : *(uint64_t*)(buf + offset);
        offset += sizeof(gnss_timestamp);

        gnss_velocity = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(gnss_velocity);

        position.longitude = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(position.longitude);

        position.latitude = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(position.latitude);

        position.elevation = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(position.elevation);

        heading = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(heading);

        gear = *(uint8_t*)(buf + offset);
        offset += sizeof(gear);

        steering_angle = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(steering_angle);

        velocity = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(velocity);

        acc_lon = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(acc_lon);

        acc_lat = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(acc_lat);

        acc_ver = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(acc_ver);
    
        yaw_rate = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(yaw_rate);

        accel_pos = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(accel_pos);

        engine_speed = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(engine_speed);

        engine_torque = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(engine_torque);

        break_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(break_flag);

        break_pos = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(break_pos);

        break_pressure = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(break_pressure);

        fuel_consume = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(fuel_consume);

        drive_mode = *(uint8_t*)(buf + offset);
        offset += sizeof(drive_mode);

        dest_location.longitude = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(dest_location.longitude);

        dest_location.latitude = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(dest_location.latitude);

        pass_pos_num = *(uint8_t*)(buf + offset);
        offset += sizeof(pass_pos_num);

        for (size_t i = 0; i < pass_pos_num; i++)
        {
            Position2D pos;

            pos.longitude = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
            offset += sizeof(pos.longitude);

            pos.latitude = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
            offset += sizeof(pos.latitude);

            pass_pos.emplace_back(pos);
        }

        return offset;
    }

    size_t to_bytes(void *_buf, const size_t _size, const bool _big_endian = true) const
    {
        if (nullptr == _buf)
        {
            LOGE(TAG, "Buffer is null!\n");
            return 0;
        }

        size_t offset = MessageHeader::to_bytes(_buf, _size, _big_endian);
        size_t len = length() - MessageHeader::length();
        if (_size - offset < len)
        {
            LOGE(TAG, "Buffer size %ld is not enough, offset %ld, data length %ld!\n", 
                _size, offset, len);
            return offset;
        }

        uint8_t *buf = (uint8_t*)_buf;

        memcpy(buf + offset, vehicle_id, sizeof(vehicle_id));
        offset += sizeof(vehicle_id);

        std::vector<uint8_t> v(message_id, message_id + sizeof(message_id));
        if (_big_endian)
        {
            std::reverse(v.begin(), v.end());
        }
        memcpy(buf + offset, v.data(), sizeof(message_id));
        offset += sizeof(message_id);

        *(uint64_t *)(buf + offset) = _big_endian ? __builtin_bswap64(gnss_timestamp) : gnss_timestamp;
        offset += sizeof(gnss_timestamp);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(gnss_velocity) : gnss_velocity;
        offset += sizeof(gnss_velocity);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(position.longitude) : position.longitude;
        offset += sizeof(position.longitude); 

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(position.latitude) : position.latitude;
        offset += sizeof(position.latitude);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(position.elevation) : position.elevation;
        offset += sizeof(position.elevation); 

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(heading) : heading;
        offset += sizeof(heading); 

        *(uint8_t *)(buf + offset) = gear;
        offset += sizeof(gear);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(steering_angle) : steering_angle;
        offset += sizeof(steering_angle);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(velocity) : velocity;
        offset += sizeof(velocity);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(acc_lon) : acc_lon;
        offset += sizeof(acc_lon);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(acc_lat) : acc_lat;
        offset += sizeof(acc_lat);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(acc_ver) : acc_ver;
        offset += sizeof(acc_ver);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(yaw_rate) : yaw_rate;
        offset += sizeof(yaw_rate);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(accel_pos) : accel_pos;
        offset += sizeof(accel_pos);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(engine_speed) : engine_speed;
        offset += sizeof(engine_speed);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(engine_torque) : engine_torque;
        offset += sizeof(engine_torque);

        *(uint8_t *)(buf + offset) = break_flag;
        offset += sizeof(break_flag);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(break_pos) : break_pos;
        offset += sizeof(break_pos);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(break_pressure) : break_pressure;
        offset += sizeof(break_pressure);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(fuel_consume) : fuel_consume;
        offset += sizeof(fuel_consume);

        *(uint8_t *)(buf + offset) = drive_mode;
        offset += sizeof(drive_mode);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(dest_location.longitude) : dest_location.longitude;
        offset += sizeof(dest_location.longitude); 

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(dest_location.latitude) : dest_location.latitude;
        offset += sizeof(dest_location.latitude);

        *(uint8_t *)(buf + offset) = pass_pos_num;
        offset += sizeof(pass_pos_num);

        for (size_t i = 0; i < pass_pos_num; i++)
        {
            *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(pass_pos[i].longitude) : pass_pos[i].longitude;
            offset += sizeof(pass_pos[i].longitude); 

            *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(pass_pos[i].latitude) : pass_pos[i].latitude;
            offset += sizeof(pass_pos[i].latitude);
        }

        return offset;
    }  

    friend std::ostream& operator<<(std::ostream &os, const Veh2CloudState &_msg)
    {
        auto base = (const MessageHeader&)_msg;

        os << base;

        uint64_t value = 0;
        size_t offset = base.length();
        size_t len = 0;
        char str[256] = "";
        char ostr[1024] = "";

        len = sizeof(_msg.vehicle_id);
        bytes_to_string(_msg.vehicle_id, len, str);
        sprintf(ostr, "%-4ld[%s] 车辆编号: %s\n", offset, str, std::string(_msg.vehicle_id, len).c_str());
        offset += len;
        os << ostr;

        len = sizeof(_msg.message_id);
        bytes_to_string(_msg.message_id, len, str);
        std::string tmp(str);
        std::reverse(tmp.begin(), tmp.end());
        sprintf(ostr, "%-4ld[%s] 消息编号: %s\n", offset, tmp.c_str(), tmp.c_str());
        offset += len;
        os << ostr;

        value = _msg.gnss_timestamp;
        sprintf(ostr, "%-4ld[%016" PRIX64 "] GNSS时间戳: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.gnss_timestamp);
        os << ostr;

        value = _msg.gnss_velocity;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] GNSS速度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.gnss_velocity);
        os << ostr;

        value = _msg.position.longitude;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 位置-经度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.position.longitude);
        os << ostr;

        value = _msg.position.latitude;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 位置-纬度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.position.latitude);
        os << ostr;

        value = _msg.position.elevation;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 位置-高程: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.position.elevation);
        os << ostr;

        value = _msg.heading;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 航向角: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.heading);
        os << ostr;

        value = _msg.gear;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 档位: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.gear);
        os << ostr;

        value = _msg.steering_angle;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 方向盘转角: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.steering_angle);
        os << ostr;

        value = _msg.velocity;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 当前车速: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.velocity);
        os << ostr;

        value = _msg.acc_lon;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 纵向加速度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.acc_lon);
        os << ostr;

        value = _msg.acc_lat;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 横向加速度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.acc_lat);
        os << ostr;

        value = _msg.acc_ver;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 垂向加速度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.acc_ver);
        os << ostr;

        value = _msg.yaw_rate;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 横摆角速度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.yaw_rate);
        os << ostr;

        value = _msg.accel_pos;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 油门开度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.accel_pos);
        os << ostr;

        value = _msg.engine_speed;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 发动机输出转速: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.engine_speed);
        os << ostr;

        value = _msg.engine_torque;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 发动机扭矩: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.engine_torque);
        os << ostr;

        value = _msg.break_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 制动踏板开关: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.break_flag);
        os << ostr;

        value = _msg.break_pos;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 制动踏板开度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.break_pos);
        os << ostr;

        value = _msg.break_pressure;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 制动主缸压力: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.break_pressure);
        os << ostr;

        value = _msg.fuel_consume;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 油耗: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.fuel_consume);
        os << ostr;

        value = _msg.drive_mode;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 车辆驾驶模式: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.drive_mode);
        os << ostr;

        value = _msg.dest_location.longitude;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 目的地位置-经度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.dest_location.longitude);
        os << ostr;

        value = _msg.dest_location.latitude;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 目的地位置-纬度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.dest_location.latitude);
        os << ostr;

        value = _msg.pass_pos_num;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 途经点数量: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.pass_pos_num);
        os << ostr;

        for (size_t i = 0; i < _msg.pass_pos_num; i++)
        {
            value = _msg.pass_pos[i].longitude;
            sprintf(ostr, "%-4ld[%08" PRIX64 "] 途经点%ld-经度: %" PRIu64 "\n", offset, value, i, value);
            offset += sizeof(_msg.pass_pos[i].longitude);
            os << ostr;

            value = _msg.pass_pos[i].latitude;
            sprintf(ostr, "%-4ld[%08" PRIX64 "] 途经点%ld-纬度: %" PRIu64 "\n", offset, value, i, value);
            offset += sizeof(_msg.pass_pos[i].latitude);
            os << ostr;
        }

        return os;
    }

    static constexpr const char *TAG = "csae295::Veh2CloudState";

    char                    vehicle_id[8] = "";
    uint8_t                 message_id[8] = {0};
    uint64_t                gnss_timestamp;
    uint16_t                gnss_velocity;
    Position                position;
    uint32_t                heading;
    uint8_t                 gear;
    uint32_t                steering_angle;
    uint16_t                velocity;
    uint16_t                acc_lon;
    uint16_t                acc_lat;
    uint16_t                acc_ver;
    uint16_t                yaw_rate;
    uint16_t                accel_pos;
    uint16_t                engine_speed;
    uint32_t                engine_torque;
    uint8_t                 break_flag;
    uint16_t                break_pos;
    uint16_t                break_pressure;
    uint16_t                fuel_consume;
    uint8_t                 drive_mode;
    Position2D              dest_location;
    uint8_t                 pass_pos_num;
    std::vector<Position2D> pass_pos;
};

/**
 * VEH2CLOUD_STATE version 2.
 */
struct Veh2CloudState2 : public Veh2CloudState
{
    Veh2CloudState2(
        const uint8_t _version,
        const uint64_t _timestamp,
        const uint8_t _ctrl,
        const std::string &_vehicle_id,
        const std::vector<uint8_t> &_message_id,
        const uint64_t _gnss_timestamp,
        const uint16_t _gnss_velocity,
        const Position &_position,
        const uint32_t _heading,
        const uint8_t  _gear,
        const uint32_t _steering_angle,
        const uint16_t _velocity,
        const uint16_t _acc_lon,
        const uint16_t _acc_lat,
        const uint16_t _acc_ver,
        const uint16_t _yaw_rate,
        const uint16_t _accel_pos,
        const uint16_t _engine_speed,
        const uint32_t _engine_torque,
        const uint8_t  _break_flag,
        const uint16_t _break_pos,
        const uint16_t _break_pressure,
        const uint16_t _fuel_consume,
        const uint8_t _drive_mode,
        const Position2D &_dest_location,
        const std::vector<Position2D> &_pass_pos,
        const uint8_t _abs_flag,
        const uint8_t _tcs_flag,
        const uint8_t _esp_flag,
        const uint8_t _lka_flag,
        const uint8_t _acc_flag,
        const uint8_t _fcw_flag,
        const uint8_t _ldw_flag,
        const uint8_t _aeb_flag,
        const uint8_t _lca_flag,
        const uint8_t _dms_flag,
        const uint32_t _mileage,
        const uint16_t _fuel_gauge,
        const uint16_t _soc,
        const uint8_t _temperature,
        const uint32_t _endurance,
        const uint16_t _vehfault,
        const uint16_t _motorspeed,
        const uint32_t _motortorque,
        const uint8_t _veh_mode,
        const uint8_t _charge_state,
        const uint16_t _batt_vol,
        const uint16_t _batt_cur,
        const uint8_t _horn_state,
        const std::vector<uint16_t> &_wheel_velocity,
        const std::vector<uint16_t> &_tire_pressure,
        const uint16_t _lights,
        const uint16_t _doors,
        const std::string &_userdefined_data):
            Veh2CloudState(
                _version,
                _timestamp,
                _ctrl,
                _vehicle_id,
                _message_id,
                _gnss_timestamp,
                _gnss_velocity,
                _position, 
                _heading, 
                _gear, 
                _steering_angle, 
                _velocity, 
                _acc_lon, 
                _acc_lat, 
                _acc_ver, 
                _yaw_rate, 
                _accel_pos, 
                _engine_speed, 
                _engine_torque, 
                _break_flag, 
                _break_pos, 
                _break_pressure, 
                _fuel_consume, 
                _drive_mode, 
                _dest_location, 
                _pass_pos),
            abs_flag(_abs_flag),
            tcs_flag(_tcs_flag),
            esp_flag(_esp_flag),
            lka_flag(_lka_flag),
            acc_flag(_acc_flag),
            fcw_flag(_fcw_flag),
            ldw_flag(_ldw_flag),
            aeb_flag(_aeb_flag),
            lca_flag(_lca_flag),
            dms_flag(_dms_flag),
            mileage(_mileage),
            fuel_gauge(_fuel_gauge),
            soc(_soc),
            temperature(_temperature),
            endurance(_endurance),
            vehfault(_vehfault),
            motorspeed(_motorspeed),
            motortorque(_motortorque),
            veh_mode(_veh_mode),
            charge_state(_charge_state),
            batt_vol(_batt_vol),
            batt_cur(_batt_cur),
            horn_state(_horn_state),
            wheel_num(_wheel_velocity.size()),
            wheel_velocity(_wheel_velocity),
            tire_pressure(_tire_pressure),
            lights(_lights),
            doors(_doors),
            user_data_len(_userdefined_data.length()),
            user_data(_userdefined_data)
    {
        size_t n = sizeof(vehicle_id) <= _vehicle_id.length() ? sizeof(vehicle_id) : _vehicle_id.length();
        strncpy(vehicle_id, _vehicle_id.c_str(), n);

        n = sizeof(message_id) <= _message_id.size() ? sizeof(message_id) : _message_id.size();
        memcpy(message_id, _message_id.data(), n);

        data_len = 
            sizeof(Veh2CloudState2) - sizeof(MessageHeader) 
            - sizeof(pass_pos) + pass_pos.size() 
            - sizeof(wheel_velocity) + wheel_velocity.size() 
            - sizeof(tire_pressure) + tire_pressure.size() 
            - sizeof(user_data) + user_data.length();
    }

    Veh2CloudState2(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        from_bytes(_buf, _size, _big_endian);
    }

    size_t length() const
    {
        return sizeof(Veh2CloudState2) 
            - sizeof(pass_pos) + pass_pos.size() 
            - sizeof(wheel_velocity) + wheel_velocity.size() 
            - sizeof(tire_pressure) + tire_pressure.size() 
            - sizeof(user_data) + user_data.length();
    }

    size_t from_bytes(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        if (nullptr == _buf)
        {
            LOGE(TAG, "Buffer is null!\n");
            return 0;
        }

        size_t offset = Veh2CloudState::from_bytes(_buf, _size, _big_endian);
        size_t len = length() - Veh2CloudState::length();
        if (_size - offset < len)
        {
            LOGE(TAG, "Buffer size %ld is not enough, offset %ld, data length %ld!\n", 
                _size, offset, len);
            return offset;
        }
    
        uint8_t *buf = (uint8_t*)_buf;

        abs_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(abs_flag);

        tcs_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(tcs_flag);

        esp_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(esp_flag);

        lka_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(lka_flag);

        acc_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(acc_flag);

        fcw_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(fcw_flag);

        ldw_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(ldw_flag);

        aeb_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(aeb_flag);

        lca_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(lca_flag);

        dms_flag = *(uint8_t*)(buf + offset);
        offset += sizeof(dms_flag);

        mileage = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(mileage);

        fuel_gauge = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(fuel_gauge);

        soc = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(soc);

        temperature = *(uint8_t*)(buf + offset);
        offset += sizeof(temperature);

        endurance = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(endurance);       

        vehfault = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(vehfault);

        motorspeed = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(motorspeed);

        motortorque = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(motortorque); 

        veh_mode = *(uint8_t*)(buf + offset);
        offset += sizeof(veh_mode);

        charge_state = *(uint8_t*)(buf + offset);
        offset += sizeof(charge_state);

        batt_vol = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(batt_vol);

        batt_cur = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(batt_cur);

        horn_state = *(uint8_t*)(buf + offset);
        offset += sizeof(horn_state);

        wheel_num = *(uint8_t*)(buf + offset);
        offset += sizeof(wheel_num);

        for (size_t i = 0; i < wheel_num; i++)
        {
            wheel_velocity[i] = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
            offset += sizeof(wheel_velocity[i]);
        }
        
        for (size_t i = 0; i < wheel_num; i++)
        {
            tire_pressure[i] = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
            offset += sizeof(tire_pressure[i]);
        }

        lights = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(lights);

        doors = _big_endian ? __builtin_bswap16(*(uint16_t*)(buf + offset)) : *(uint16_t*)(buf + offset);
        offset += sizeof(doors);

        user_data_len = *(uint8_t*)(buf + offset);
        offset += sizeof(user_data_len);
        if (0 != user_data_len)
        {
            user_data = std::string((char *)buf + offset, user_data_len);
            offset += user_data_len;
        }

        return offset;
    }

    size_t to_bytes(void *_buf, const size_t _size, const bool _big_endian = true) const
    {
        if (nullptr == _buf)
        {
            LOGE(TAG, "Buffer is null!\n");
            return 0;
        }

        size_t offset = Veh2CloudState::to_bytes(_buf, _size, _big_endian);
        size_t len = length() - Veh2CloudState::length();
        if (_size - offset < len)
        {
            LOGE(TAG, "Buffer size %ld is not enough, offset %ld, data length %ld!\n", 
                _size, offset, len);
            return offset;
        }

        uint8_t *buf = (uint8_t*)_buf;

        *(uint8_t *)(buf + offset) = abs_flag;
        offset += sizeof(abs_flag);

        *(uint8_t *)(buf + offset) = tcs_flag;
        offset += sizeof(tcs_flag);

        *(uint8_t *)(buf + offset) = esp_flag;
        offset += sizeof(esp_flag);

        *(uint8_t *)(buf + offset) = lka_flag;
        offset += sizeof(lka_flag);

        *(uint8_t *)(buf + offset) = acc_flag;
        offset += sizeof(acc_flag);

        *(uint8_t *)(buf + offset) = fcw_flag;
        offset += sizeof(fcw_flag);

        *(uint8_t *)(buf + offset) = ldw_flag;
        offset += sizeof(ldw_flag);

        *(uint8_t *)(buf + offset) = aeb_flag;
        offset += sizeof(aeb_flag);

        *(uint8_t *)(buf + offset) = lca_flag;
        offset += sizeof(lca_flag);

        *(uint8_t *)(buf + offset) = dms_flag;
        offset += sizeof(dms_flag);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(mileage) : mileage;
        offset += sizeof(mileage); 

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(fuel_gauge) : fuel_gauge;
        offset += sizeof(fuel_gauge);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(soc) : soc;
        offset += sizeof(soc);

        *(uint8_t *)(buf + offset) = temperature;
        offset += sizeof(temperature);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(endurance) : endurance;
        offset += sizeof(endurance); 

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(vehfault) : vehfault;
        offset += sizeof(vehfault);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(motorspeed) : motorspeed;
        offset += sizeof(motorspeed);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(motortorque) : motortorque;
        offset += sizeof(motortorque);

        *(uint8_t *)(buf + offset) = veh_mode;
        offset += sizeof(veh_mode);

        *(uint8_t *)(buf + offset) = charge_state;
        offset += sizeof(charge_state);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(batt_vol) : batt_vol;
        offset += sizeof(batt_vol);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(batt_cur) : batt_cur;
        offset += sizeof(batt_cur);

        *(uint8_t *)(buf + offset) = horn_state;
        offset += sizeof(horn_state);

        *(uint8_t *)(buf + offset) = wheel_num;
        offset += sizeof(wheel_num);

        for (size_t i = 0; i < wheel_num; i++)
        {
            *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(wheel_velocity[i]) : wheel_velocity[i];
            offset += sizeof(wheel_velocity[i]);
        }

        for (size_t i = 0; i < wheel_num; i++)
        {
            *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(tire_pressure[i]) : tire_pressure[i];
            offset += sizeof(tire_pressure[i]);
        }

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(lights) : lights;
        offset += sizeof(lights);

        *(uint16_t *)(buf + offset) = _big_endian ? __builtin_bswap16(doors) : doors;
        offset += sizeof(doors);
        
        *(uint8_t *)(buf + offset) = user_data_len;
        offset += sizeof(user_data_len);
        if (0 != user_data_len)
        {
            memcpy(buf + offset, user_data.c_str(), user_data_len);
            offset += user_data_len;
        }

        return offset;
    }  

    friend std::ostream& operator<<(std::ostream &os, const Veh2CloudState2 &_msg)
    {
        auto base = (const Veh2CloudState&)_msg;

        os << base;

        uint64_t value = 0;
        size_t offset = base.length();
        size_t len = 0;
        char str[256] = "";
        char ostr[1024] = "";

        value = _msg.abs_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] ABS状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.abs_flag);
        os << ostr;

        value = _msg.tcs_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] TCS状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.tcs_flag);
        os << ostr;

        value = _msg.esp_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] ESP状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.esp_flag);
        os << ostr;

        value = _msg.lka_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] LKA状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.lka_flag);
        os << ostr;

        value = _msg.acc_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] ACC工作模式: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.acc_flag);
        os << ostr;

        value = _msg.fcw_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] FCW状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.fcw_flag);
        os << ostr;

        value = _msg.ldw_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] LDW状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.ldw_flag);
        os << ostr;

        value = _msg.aeb_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] AEB状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.aeb_flag);
        os << ostr;

        value = _msg.lca_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] LCA状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.lca_flag);
        os << ostr;

        value = _msg.dms_flag;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] DMS状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.dms_flag);
        os << ostr;

        value = _msg.mileage;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 里程: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.mileage);
        os << ostr;

        value = _msg.fuel_gauge;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 油量: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.fuel_gauge);
        os << ostr;

        value = _msg.soc;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 电池剩余电量: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.soc);
        os << ostr;

        value = _msg.temperature;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 电池温度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.temperature);
        os << ostr;

        value = _msg.endurance;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 预计续航里程: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.endurance);
        os << ostr;
        
        value = _msg.vehfault;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 车辆故障状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.vehfault);
        os << ostr;

        value = _msg.motorspeed;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 电机转速: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.motorspeed);
        os << ostr;

        value = _msg.motortorque;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 电机转矩: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.motortorque);
        os << ostr;

        value = _msg.veh_mode;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 运行模式: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.veh_mode);
        os << ostr;

        value = _msg.charge_state;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 充电状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.charge_state);
        os << ostr;

        value = _msg.batt_vol;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 动力电池实时电压: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.batt_vol);
        os << ostr;

        value = _msg.batt_cur;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 动力电池实时电流: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.batt_cur);
        os << ostr;

        value = _msg.horn_state;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 喇叭状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.horn_state);
        os << ostr;

        value = _msg.wheel_num;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 车轮数: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.wheel_num);
        os << ostr;

        for (size_t i = 0; i < _msg.wheel_num; i++)
        {
            value = _msg.wheel_velocity[i];
            sprintf(ostr, "%-4ld[%04" PRIX64 "] 车轮%ld-轮速: %" PRIu64 "\n", offset, value, i, value);
            offset += sizeof(_msg.wheel_velocity[i]);
            os << ostr;
        }
        
        for (size_t i = 0; i < _msg.wheel_num; i++)
        {
            value = _msg.tire_pressure[i];
            sprintf(ostr, "%-4ld[%04" PRIX64 "] 车轮%ld-胎压: %" PRIu64 "\n", offset, value, i, value);
            offset += sizeof(_msg.tire_pressure[i]);
            os << ostr;
        }

        value = _msg.lights;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 车灯状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.lights);
        os << ostr;

        value = _msg.doors;
        sprintf(ostr, "%-4ld[%04" PRIX64 "] 车门状态: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.doors);
        os << ostr;

        value = _msg.user_data_len;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 自定义字段长度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.user_data_len);
        os << ostr;

        if (0 != _msg.user_data_len && 0xFF !=  _msg.user_data_len)
        {
            len = _msg.user_data_len;
            bytes_to_string(_msg.user_data.c_str(), len, str);
            sprintf(ostr, "%-4ld[%s] 自定义字段内容: %s\n", offset, str, _msg.user_data.c_str());
            offset += len;
            os << ostr;
        }

        return os;
    }

    static constexpr const char *TAG = "csae295::Veh2CloudState2";

    uint8_t                 abs_flag;
    uint8_t                 tcs_flag;
    uint8_t                 esp_flag;
    uint8_t                 lka_flag;
    uint8_t                 acc_flag;
    uint8_t                 fcw_flag;
    uint8_t                 ldw_flag;
    uint8_t                 aeb_flag;
    uint8_t                 lca_flag;
    uint8_t                 dms_flag;
    uint32_t                mileage;
    uint16_t                fuel_gauge;
    uint16_t                soc;
    uint8_t                 temperature;
    uint32_t                endurance;
    uint16_t                vehfault;
    uint16_t                motorspeed;
    uint32_t                motortorque;
    uint8_t                 veh_mode;
    uint8_t                 charge_state;
    uint16_t                batt_vol;
    uint16_t                batt_cur;
    uint8_t                 horn_state;
    uint8_t                 wheel_num;
    std::vector<uint16_t>   wheel_velocity;
    std::vector<uint16_t>   tire_pressure;
    uint16_t                lights;
    uint16_t                doors;
    uint8_t                 user_data_len;
    std::string             user_data;
};

#pragma pack()
} // namespace csae295

#endif // __CSAE295_VEH2CLOUD_STATE_H__