#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/

void print_buffer(const char *_prefix, const uint32_t _id, const void *_buf, const size_t _size);

void bytes_to_string(const void *_bytes, const size_t _size, char *_str);

/**
 * 将十六进制字符串转换为字节数组。
 * 
 * @param _str   输入的十六进制字符串（每个字节由两个连续十六进制字符表示，字节之间用空白分隔）
 * @param _bytes 输出缓冲区
 * @param _size  输出缓冲区大小（字节数）
 * @return       实际写入缓冲区的字节数（若缓冲区不足则只写入前 _size 个字节）
 */
size_t string_to_bytes(const char *_str, void *_bytes, const size_t _size);

void string_to_bcd(const char *_str, void *_bcd, const size_t _size);

/**
 * 将 BCD 编码的北京时间（UTC+8）转换为 UTC 毫秒时间戳。
 *
 * @param _bcd  指向 6 字节 BCD 时间数组的指针，顺序为：年,月,日,时,分,秒。
 * @param _size 数组长度（应 >= 6）。
 * @return      UTC 毫秒时间戳；若参数无效返回 0。
 */
uint64_t bcd_time_to_utc_milliseconds(const uint8_t *_bcd, const size_t _size);

int32_t code_convert(
    const char *_from_charset, 
    const char *_to_charset, 
    const char *_inbuf, 
    const size_t _inlen, 
    char *_outbuf, 
    size_t _outlen);

int32_t utf8_to_gbk(const char *_inbuf, const size_t _inlen, char *_outbuf, size_t _outlen);

int32_t gbk_to_utf8(const char *_inbuf, const size_t _inlen, char *_outbuf, size_t _outlen);

#ifdef __cplusplus
}
#endif

#endif // __CONVERTER_H__
