#ifndef __CSAE295_MESSAGE_H__
#define __CSAE295_MESSAGE_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <cinttypes>

#include "util/converter.h"
#include "util/log.h"

/**
 * @name Data type
 * @{
 */
#define VEH2CLOUD_INH       0x34
#define CLOUD2VEH_INH_RES   0x35
#define VEH2CLOUD_STATE     0x15
#define HEARTBEAT           0x0C
#define HEARTBEAT_RES       0x0D
/** @} */ // Data type

/**
 * @name Data version
 * @{
 */
#define VERSION_01  0x01
#define VERSION_02  0x02
#define VERSION_03  0x03
/** @} */ // Data version

/**
 * @name Control content
 * @{
 */
#define CTRL_PRIORITY_0 (0 << 2)
#define CTRL_PRIORITY_1 (1 << 2)
#define CTRL_PRIORITY_2 (2 << 2)
#define CTRL_PRIORITY_3 (3 << 2)
#define CTRL_PRIORITY_4 (4 << 2)
#define CTRL_PRIORITY_5 (5 << 2)
#define CTRL_PRIORITY_6 (6 << 2)
#define CTRL_PRIORITY_7 (7 << 2)

#define CTRL_ENCRYPTION_NONE    (0 << 5)
#define CTRL_ENCRYPTION_AES     (1 << 5)
#define CTRL_ENCRYPTION_SM4     (2 << 5)
#define CTRL_ENCRYPTION_SM2     (3 << 5)
#define CTRL_ENCRYPTION_SM3     (4 << 5)
#define CTRL_ENCRYPTION_RSA     (5 << 5)
#define CTRL_ENCRYPTION_X509    (6 << 5)
#define CTRL_ENCRYPTION_RESERVE (7 << 5)

/** @} */ // Control content


namespace csae295
{
#pragma pack(1)

/**
 * Message buffer.
 */
struct MessageBuffer
{
    template<typename T>
    inline static void deleter(T *_p)
    {	
        delete [](uint8_t*)_p;
    }

    uint32_t size;
    uint8_t  data[0];
};

/**
 * Message header.
 */
struct MessageHeader
{
    MessageHeader() {}

    MessageHeader(
        const uint32_t _data_len,
        const uint8_t _data_type,
        const uint8_t _version,
        const uint64_t _timestamp,
        const uint8_t _ctrl): 
            data_len(_data_len), 
            data_type(_data_type), 
            version(_version), 
            timestamp(_timestamp), 
            ctrl(_ctrl) {}

    MessageHeader(const void *_buf, const size_t _size, const bool _big_endian = true) 
    {
        from_bytes(_buf, _size, _big_endian);
    }

    size_t header_length() const
    {           
        return sizeof(MessageHeader);
    }

    size_t data_length() const
    {
        return data_len;
    }

    size_t length() const
    {
        return sizeof(MessageHeader);
    }

    size_t from_bytes(const void *_buf, const size_t _size, const bool _big_endian = true)
    {
        if (nullptr == _buf)
        {
            LOGE(TAG, "Buffer is null!\n");
            return 0;
        }

        size_t len = length();
        if (_size < len)
        {
            LOGE(TAG, "Buffer size %ld is not enough, header length %ld!\n", 
                _size, len);
            return 0;
        }

        size_t offset = 0;
        uint8_t *buf = (uint8_t*)_buf;

        id = *(uint8_t*)(buf + offset);
        offset += sizeof(id);

        data_len = _big_endian ? __builtin_bswap32(*(uint32_t*)(buf + offset)) : *(uint32_t*)(buf + offset);
        offset += sizeof(data_len);

        data_type = *(uint8_t*)(buf + offset);
        offset += sizeof(data_type);

        version = *(uint8_t*)(buf + offset);
        offset += sizeof(version);

        timestamp = _big_endian ? __builtin_bswap64(*(uint64_t*)(buf + offset)) : *(uint64_t*)(buf + offset);
        offset += sizeof(timestamp);

        ctrl = *(uint8_t*)(buf + offset);
        offset += sizeof(ctrl);

        return offset;
    }

    size_t to_bytes(void *_buf, const size_t _size, const bool _big_endian = true) const
    {
        if (nullptr == _buf)
        {
            LOGE(TAG, "Buffer is null!\n");
            return 0;
        }

        size_t len = length();
        if (_size < len)
        {
            LOGE(TAG, "Buffer size %ld is not enough, header length %ld!\n", 
                _size, len);
            return 0;
        }

        size_t offset = 0;
        uint8_t *buf = (uint8_t*)_buf;

        *(uint8_t*)(buf + offset) = id;
        offset += sizeof(id);

        *(uint32_t *)(buf + offset) = _big_endian ? __builtin_bswap32(data_len) : data_len;
        offset += sizeof(data_len);

        *(uint8_t*)(buf + offset) = data_type;
        offset += sizeof(data_type);

        *(uint8_t*)(buf + offset) = version;
        offset += sizeof(version);

        *(uint64_t *)(buf + offset) = _big_endian ? __builtin_bswap64(timestamp) : timestamp;
        offset += sizeof(timestamp);

        *(uint8_t*)(buf + offset) = ctrl;
        offset += sizeof(ctrl);
        
        return offset;
    }

    friend std::ostream& operator<<(std::ostream &os, const MessageHeader &_header)
    {
        uint64_t value = 0;
        size_t offset = 0;
        char ostr[1024] = "";

        value = _header.id;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 标识位: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_header.id);
        os << ostr;

        value = _header.data_len;
        sprintf(ostr, "%-4ld[%08" PRIX64 "] 数据段长度: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_header.data_len);
        os << ostr;

        value = _header.data_type;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 数据类别: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_header.data_type);
        os << ostr;

        value = _header.version;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 版本号: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_header.version);
        os << ostr;

        value = _header.timestamp;
        sprintf(ostr, "%-4ld[%016" PRIX64 "] 时间戳: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_header.timestamp);
        os << ostr;

        value = _header.ctrl;
        sprintf(ostr, "%-4ld[%02" PRIX64 "] 控制内容: %" PRIu64 "\n", offset, value, value);
        offset += sizeof(_header.ctrl);
        os << ostr;

        return os;
    }

    static constexpr const char *TAG = "csae295::MessageHeader";

    uint8_t  id = 0xF2;
    uint32_t data_len;
    uint8_t  data_type;
    uint8_t  version;
    uint64_t timestamp;
    uint8_t  ctrl;
};

#pragma pack()
} // namespace csae295

#endif // __CSAE295_MESSAGE_H__
