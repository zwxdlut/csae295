#include "util/converter.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include <iconv.h>

#include "util/log.h"

#define TAG "converter"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/**
 * 将十六进制字符转换为数值 (0~15)，非法字符返回 -1
 */
static int32_t hex_char_to_val(char c);

/*******************************************************************************
 * Functions
 ******************************************************************************/

void print_buffer(const char *_prefix, const uint32_t _id, const void *_buf, const size_t _size)
{
#ifdef _UDEBUG
    if (NULL == _buf || 0 == _size)
    {
        LOGE("BUFFER", "%s: Buffer is null!\n", _prefix);
    }

    char str[4 * _size + strlen(_prefix) + 100];
    uint8_t *buf = (uint8_t*)_buf;
    
    sprintf(str, "%s(0x%X,%ld): ", _prefix, _id, _size);

    for (size_t i = 0; i < _size; i++)
    {
        char s[10] = "";

        sprintf(s, "%02X ", buf[i]);
        strcat(str, s);
    }

    strcat(str, "\n");

    LOGD("BUFFER", "%s", str);
#endif
}

void bytes_to_string(const void *_bytes, const size_t _size, char *_str)
{
    if (NULL == _bytes || NULL == _str || 0 == _size)
    {
        LOGE(TAG, "Bytes or string is null!\n");
    }

    uint8_t *bytes = (uint8_t*)_bytes;
    
    strcpy(_str, "");
    
    for (size_t i = 0; i < _size; i++)
    {
        char s[3] = "";

        sprintf(s, "%02X", bytes[i]);
        strcat(_str, s);
    }		
}

size_t string_to_bytes(const char *_str, void *_bytes, const size_t _size)
{
    // 参数校验
    if (_str == NULL || _bytes == NULL || _size == 0) 
    {
        LOGE(TAG, "Bytes or string is null!\n");
        return 0;
    }

    const char *p = _str;
    uint8_t *out = (uint8_t *)_bytes;
    size_t count = 0;

    memset(out, 0, _size);

    while (*p) 
    {
        // 跳过空白字符
        while (*p && isspace((unsigned char)*p)) 
        {
            p++;
        }
        if (*p == '\0') 
        {
            break;               // 字符串结束
        }

        // 读取第一个十六进制字符
        int hi = hex_char_to_val(*p);
        if (hi < 0) 
        {
            break;               // 非法字符，停止解析
        }
        p++;

        // 读取第二个十六进制字符
        if (*p == '\0') 
        {
            break;               // 奇数个字符，格式错误
        }
        int lo = hex_char_to_val(*p);
        if (lo < 0) 
        {
            break;               // 非法字符
        }
        p++;

        // 合成一个字节
        uint8_t byte = (uint8_t)((hi << 4) | lo);

        // 若缓冲区还有空间则写入，否则停止
        if (count < _size) 
        {
            out[count] = byte;
            count++;
        } 
        else 
        {
            break;               // 缓冲区已满
        }
    }

    return count;
}

void string_to_bcd(const char *_str, void *_bcd, const size_t _size)
{
    if (NULL == _bcd)
    {       
        LOGE(TAG, "Buffer is null!\n"); 
    }

    uint8_t *bcd = (uint8_t *)_bcd;
    size_t len = strlen(_str);
    bool is_even = !(len % 2);

    memset(bcd, 0, _size);

    for (size_t i = 0; i < len; ++i)
    {
        char c = _str[i];
        int value = atoi(&c);
        int j = (2 * _size - len + i) / 2;

        if (j >= _size)
        {
            LOGE(TAG, "Out of range!\n"); 
            return;
        }
        
        bcd[(2 * _size - len + i) / 2] |= value << (4 * ((is_even ? i + 1 : i) % 2));
    }
}

uint64_t bcd_time_to_utc_milliseconds(const uint8_t *_bcd, const size_t _size)
{
    // 参数检查
    if (_bcd == NULL || _size < 6) 
    {
        return 0;
    }

    // 提取各字段
    uint8_t year_bcd  = _bcd[0];
    uint8_t month_bcd = _bcd[1];
    uint8_t day_bcd   = _bcd[2];
    uint8_t hour_bcd  = _bcd[3];
    uint8_t min_bcd   = _bcd[4];
    uint8_t sec_bcd   = _bcd[5];

    // BCD 转十进制
    int year  = (year_bcd  >> 4) * 10 + (year_bcd  & 0x0F) + 2000;
    int month = (month_bcd >> 4) * 10 + (month_bcd & 0x0F);
    int day   = (day_bcd   >> 4) * 10 + (day_bcd   & 0x0F);
    int hour  = (hour_bcd  >> 4) * 10 + (hour_bcd  & 0x0F);
    int min   = (min_bcd   >> 4) * 10 + (min_bcd   & 0x0F);
    int sec   = (sec_bcd   >> 4) * 10 + (sec_bcd   & 0x0F);

    // 计算从 1970-01-01 到给定日期的天数（Howard Hinnant 算法）
    int y = year;
    int m = month;
    int d = day;
    y -= m <= 2;
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);      // [0, 399]
    unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy; // [0, 146096]
    int64_t days = era * 146097 + (int64_t)doe - 719468;

    // 北京时间转 UTC：先计算北京时间的总秒数，再减去 8 小时偏移
    int64_t total_seconds_bj = days * 86400 + (int64_t)hour * 3600 + (int64_t)min * 60 + sec;
    int64_t total_seconds_utc = total_seconds_bj - 8 * 3600;  // UTC+8 → UTC

    // 转为毫秒并返回
    return (uint64_t)(total_seconds_utc * 1000);
}

int32_t code_convert(
    const char *_from_charset, 
    const char *_to_charset, 
    const char *_inbuf, 
    const size_t _inlen, 
    char *_outbuf, 
    size_t _outlen) 
{
    iconv_t cd;
    char **pin = (char**)&_inbuf;
    char **pout = &_outbuf;

    cd = iconv_open(_to_charset, _from_charset);
    if (cd == 0)
    {
        return -1;
    }

    memset(_outbuf, 0, _outlen);
    if (iconv(cd, pin, (size_t*)&_inlen, pout, &_outlen) != 0)
    {
        return -1;
    }

    iconv_close(cd);
    *pout = '\0';

    return 0;
}

int32_t utf8_to_gbk(const char *_inbuf, const size_t _inlen, char *_outbuf, size_t _outlen) 
{
    return code_convert("utf-8", "gb2312", _inbuf, _inlen, _outbuf, _outlen);
}

int32_t gbk_to_utf8(const char *_inbuf, const size_t _inlen, char *_outbuf, size_t _outlen) 
{
    return code_convert("gb2312", "utf-8", _inbuf, _inlen, _outbuf, _outlen);
}

/*******************************************************************************
 * Local functions
 ******************************************************************************/

int32_t hex_char_to_val(char c) 
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}