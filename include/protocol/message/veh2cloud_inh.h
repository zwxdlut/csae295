#ifndef __CSAE295_VEH2CLOUD_INH_H__
#define __CSAE295_VEH2CLOUD_INH_H__

#include "protocol/message/message.h"

/**
 * @name Communication type
 * @{
 */
#define COMM_TYPE_4G        0 
#define COMM_TYPE_5G        1 
#define COMM_TYPE_OTHER     2 
#define COMM_TYPE_DEFAULT   0xFF
/** @} */ // Communication type

/**
 * @name Time sync mode
 * @{
 */
#define TIME_SYNC_UNKNOWN   0
#define TIME_SYNC_PTP       1
#define TIME_SYNC_GNSS      2
#define TIME_SYNC_NTP       3
#define TIME_SYNC_INTERNET  4
#define TIME_SYNC_OTHER     5
#define TIME_SYNC_DEFAULT   0xFF 
/** @} */ // Time sync mode

/**
 * @name GNSS type
 * @{
 */
#define GNSS_TYPE_GCJ02 0
#define GNSS_TYPE_USER  1
/** @} */ // GNSS type

/**
 * @name CLOUD2VEH_INH_RES ype
 * @{
 */
#define CLOUD2VEH_INH_RES_COMFIRM       0
#define CLOUD2VEH_INH_RES_FAIL          1
#define CLOUD2VEH_INH_RES_MSG_ERROR     2 
#define CLOUD2VEH_INH_RES_UNSUPPORTED   3
/** @} */ // CLOUD2VEH_INH_RES type

namespace csae295
{
#pragma pack(1)

/**
 * VEH2CLOUD_INH.
 */
struct Veh2CloudInh : public MessageHeader
{
    Veh2CloudInh() {}

    Veh2CloudInh(
        const uint8_t _version,
        const uint64_t _timestamp,
        const uint8_t _ctrl,
        const std::string &_vehicle_id,
        const std::string &_sw_ver,
        const std::string &_hw_ver,
        const std::string &_ad_ver,
        const uint8_t _com_type,
        const uint8_t _pos_confidence,
        const uint8_t _time_sync,
        const uint8_t _gnss_type,
        const std::string &_user_data): 
            MessageHeader(
                sizeof(Veh2CloudInh) 
                    - sizeof(MessageHeader) 
                    - sizeof(sw_ver) 
                    - sizeof(hw_ver) 
                    - sizeof(ad_ver) 
                    - sizeof(user_data), 
                VEH2CLOUD_INH, 
                _version, 
                _timestamp, 
                _ctrl), 
                sw_ver(_sw_ver), 
                hw_ver(_hw_ver), 
                ad_ver(_ad_ver), 
                com_type(_com_type), 
                pos_confidence(_pos_confidence), 
                time_sync(_time_sync), 
                gnss_type(_gnss_type), 
                user_data(_user_data)
    {
        size_t n = sizeof(vehicle_id) <= _vehicle_id.length() ? sizeof(vehicle_id) : _vehicle_id.length();
        strncpy(vehicle_id, _vehicle_id.c_str(), n);
 
        sw_ver_len = sw_ver.length() >= 0xFF ? 0 : sw_ver.length();
        data_len += sw_ver_len;

        hw_ver_len = hw_ver.length() >= 0xFF ? 0 : hw_ver.length();
        data_len += hw_ver_len;

        ad_ver_len = ad_ver.length() >= 0xFF ? 0 : ad_ver.length();
        data_len += ad_ver_len;

        user_data_len = user_data.length() >= 0xFF ? 0 : user_data.length();
        data_len += user_data_len;
    }

    Veh2CloudInh(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        from_bytes(_buf, _size, _big_endian);
    }

    size_t length() const
    {
        return sizeof(Veh2CloudInh) 
            - sizeof(sw_ver) + sw_ver.length() 
            - sizeof(hw_ver) + hw_ver.length() 
            - sizeof(ad_ver) + ad_ver.length() 
            - sizeof(user_data) + user_data.length();
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

        sw_ver_len = *(uint8_t*)(buf + offset);
        offset += sizeof(sw_ver_len);
        if (0 != sw_ver_len && 0xFF !=  sw_ver_len)
        {
            sw_ver = std::string((char*)buf + offset, sw_ver_len);
            offset += sw_ver_len;
        }

        hw_ver_len = *(uint8_t*)(buf + offset);
        offset += sizeof(hw_ver_len);
        if (0 != hw_ver_len && 0xFF !=  hw_ver_len)
        {
            hw_ver = std::string((char*)buf + offset, hw_ver_len);
            offset += hw_ver_len;
        }

        ad_ver_len = *(uint8_t*)(buf + offset);
        offset += sizeof(ad_ver_len);
        if (0 != ad_ver_len && 0xFF !=  ad_ver_len)
        {
            ad_ver = std::string((char*)buf + offset, ad_ver_len);
            offset += ad_ver_len;
        }

        com_type = *(uint8_t*)(buf + offset);
        offset += sizeof(com_type);

        pos_confidence = *(uint8_t*)(buf + offset);
        offset += sizeof(pos_confidence);

        time_sync = *(uint8_t*)(buf + offset);
        offset += sizeof(time_sync);

        gnss_type = *(uint8_t*)(buf + offset);
        offset += sizeof(gnss_type);

        user_data_len = *(uint8_t*)(buf + offset);
        offset += sizeof(user_data_len);
        if (0 != user_data_len)
        {
            user_data = std::string((char*)buf + offset, user_data_len);
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

        *(uint8_t*)(buf + offset) = sw_ver_len;
        offset += sizeof(sw_ver_len);
        if (0 != sw_ver_len && 0xFF !=  sw_ver_len)
        {
            memcpy(buf + offset, sw_ver.c_str(), sw_ver_len);
            offset += sw_ver_len;
        }

        *(uint8_t *)(buf + offset) = sw_ver_len;
        offset += sizeof(hw_ver_len);
        if (0 != hw_ver_len && 0xFF !=  hw_ver_len)
        {
            memcpy(buf + offset, sw_ver.c_str(), hw_ver_len);
            offset += hw_ver_len;
        }

        *(uint8_t *)(buf + offset) = ad_ver_len;
        offset += sizeof(ad_ver_len);
        if (0 != ad_ver_len && 0xFF !=  ad_ver_len)
        {
            memcpy(buf + offset, sw_ver.c_str(), ad_ver_len);
            offset += ad_ver_len;
        }

        *(uint8_t *)(buf + offset) = com_type;
        offset += sizeof(com_type);

        *(uint8_t *)(buf + offset) = pos_confidence;
        offset += sizeof(pos_confidence);

        *(uint8_t *)(buf + offset) = time_sync;
        offset += sizeof(time_sync);

        *(uint8_t *)(buf + offset) = gnss_type;
        offset += sizeof(gnss_type);

        *(uint8_t *)(buf + offset) = user_data_len;
        offset += sizeof(user_data_len);
        if (0 != user_data_len)
        {
            memcpy(buf + offset, user_data.c_str(), user_data_len);
            offset += user_data_len;
        }

        return offset;
    }  

    friend std::ostream& operator<<(std::ostream &os, const Veh2CloudInh &_msg)
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

        value = _msg.sw_ver_len;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 车载终端设备软件版本号长度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.sw_ver_len);
        os << ostr;

        if (0 != _msg.sw_ver_len && 0xFF !=  _msg.sw_ver_len)
        {
            len = _msg.sw_ver_len;
            bytes_to_string(_msg.sw_ver.c_str(), len, str);
            sprintf(ostr, "%-4ld[%s] 车辆软件版本: %s\n", offset, str, _msg.sw_ver.c_str());
            offset += len;
            os << ostr;
        }

        value = _msg.hw_ver_len;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 自动驾驶系统硬件版本号长度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.hw_ver_len);
        os << ostr;

        if (0 != _msg.hw_ver_len && 0xFF !=  _msg.hw_ver_len)
        {
            len = _msg.hw_ver_len;
            bytes_to_string(_msg.hw_ver.c_str(), len, str);
            sprintf(ostr, "%-4ld[%s] 自动驾驶系统硬件版本: %s\n", offset, str, _msg.hw_ver.c_str());
            offset += len;
            os << ostr;
        }

        value = _msg.ad_ver_len;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 自动驾驶系统软件版本号长度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.ad_ver_len);
        os << ostr;

        if (0 != _msg.ad_ver_len && 0xFF !=  _msg.ad_ver_len)
        {
            len = _msg.ad_ver_len;
            bytes_to_string(_msg.ad_ver.c_str(), len, str);
            sprintf(ostr, "%-4ld[%s] 自动驾驶系统软件版本号: %s\n", offset, str, _msg.ad_ver.c_str());
            offset += len;
            os << ostr;
        }

        value = _msg.com_type;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 无线通讯类型: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.com_type);
        os << ostr;

        value = _msg.pos_confidence;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 定位精度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.pos_confidence);
        os << ostr;

        value = _msg.time_sync;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 时间同步方式: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.time_sync);
        os << ostr;

        value = _msg.gnss_type;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 坐标系类型: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.gnss_type);
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

    static constexpr const char *TAG = "csae295::Veh2CloudInh";

    char        vehicle_id[8] = "";
    uint8_t     sw_ver_len;
    std::string sw_ver;
    uint8_t     hw_ver_len;
    std::string hw_ver;
    uint8_t     ad_ver_len;
    std::string ad_ver;
    uint8_t     com_type;
    uint8_t     pos_confidence;
    uint8_t     time_sync;
    uint8_t     gnss_type;
    uint8_t     user_data_len;
    std::string user_data;
};

/**
 * CLOUD2VEH_INH_RES.
 */
struct Cloud2VehInhRes : public MessageHeader
{
    Cloud2VehInhRes() {}

    Cloud2VehInhRes(
        const uint8_t _version,
        const uint64_t _timestamp,
        const uint8_t _ctrl, 
        const std::string &_vehicle_id,
        const uint8_t _res):
            MessageHeader(
                sizeof(Cloud2VehInhRes) - sizeof(MessageHeader), 
                CLOUD2VEH_INH_RES, 
                _version, 
                _timestamp, 
                _ctrl), 
            res(_res)
    {
        size_t n = sizeof(vehicle_id) <= _vehicle_id.length() ? sizeof(vehicle_id) : _vehicle_id.length();
        strncpy(vehicle_id, _vehicle_id.c_str(), n);
    }

    Cloud2VehInhRes(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        from_bytes(_buf, _size, _big_endian);
    }

    size_t length() const
    {
        return sizeof(Cloud2VehInhRes);
    }

    size_t from_bytes(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        size_t offset = MessageHeader::from_bytes(_buf, _size, _big_endian);
        if (_size - offset < data_len)
        {
            LOGE(TAG, "Buffer size %ld is lower than header length %ld + data length %d!\n", 
                _size, offset, data_len);
            return offset;
        }

        uint8_t *buf = (uint8_t*)_buf;

        memcpy(vehicle_id, buf + offset, sizeof(vehicle_id));
        offset += sizeof(vehicle_id);

        res = *(uint8_t*)(buf + offset);
        offset += sizeof(res);

        return offset;
    }

    size_t to_bytes(void *_buf, const size_t _size, const bool _big_endian = true) const
    {
        size_t offset = MessageHeader::to_bytes(_buf, _size);
        if (_size - offset < data_len)
        {
            LOGE(TAG, "Buffer size %ld is lower than header length %ld + data length %d!\n", 
                _size, offset, data_len);
            return offset;
        }

        uint8_t *buf = (uint8_t*)_buf;

        memcpy(buf + offset, vehicle_id, sizeof(vehicle_id));
        offset += sizeof(vehicle_id);

        *(uint8_t *)(buf + offset) = res;
        offset += sizeof(res);

        return offset;
    }  

    friend std::ostream& operator<<(std::ostream &os, const Cloud2VehInhRes &_msg)
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

        value = _msg.res;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 执行标志: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_msg.res);
        os << ostr;

        return os;
    }

    static constexpr const char *TAG = "csae295::Cloud2VehInhRes";

    char        vehicle_id[8];
    uint8_t     res;
};

#pragma pack()
} // namespace csae295

#endif // __CSAE295_VEH2CLOUD_INH_H__