#ifndef __UTIL_H__
#define __UTIL_H__

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>

// 获取当前UTC毫秒时间戳
inline uint64_t get_utc_timestamp_ms() 
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
}

// 获取当前时间字符串
inline std::string get_time_string() 
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

// 地理信息

// 常量定义
constexpr double PI = 3.14159265358979323846;
constexpr double A = 6378245.0;                     // 克拉索夫斯基椭球长半轴
constexpr double EE = 0.00669342162296594323;       // 椭球第一偏心率的平方

// 判断坐标是否在中国境外（粗略判断）
static inline bool out_of_china(const double _lat, const double _lon) 
{
    if (_lon < 72.004 || _lon > 137.8347) return true;
    if (_lat < 0.8293 || _lat > 55.8271) return true;
    return false;
}

// 纬度偏移计算
static inline double transform_lat(const double _x, const double _y) 
{
    double ret = -100.0 + 2.0 * _x + 3.0 * _y + 0.2 * _y * _y + 
                 0.1 * _x * _y + 0.2 * std::sqrt(std::abs(_x));
    ret += (20.0 * std::sin(6.0 * _x * PI) + 20.0 * std::sin(2.0 * _x * PI)) * 2.0 / 3.0;
    ret += (20.0 * std::sin(_y * PI) + 40.0 * std::sin(_y / 3.0 * PI)) * 2.0 / 3.0;
    ret += (160.0 * std::sin(_y / 12.0 * PI) + 320.0 * std::sin(_y * PI / 30.0)) * 2.0 / 3.0;
    return ret;
}

// 经度偏移计算
static inline double transform_lon(const double _x, const double _y) 
{
    double ret = 300.0 + _x + 2.0 * _y + 0.1 * _x * _x + 
                 0.1 * _x * _y + 0.1 * std::sqrt(std::abs(_x));
    ret += (20.0 * std::sin(6.0 * _x * PI) + 20.0 * std::sin(2.0 * _x * PI)) * 2.0 / 3.0;
    ret += (20.0 * std::sin(_x * PI) + 40.0 * std::sin(_x / 3.0 * PI)) * 2.0 / 3.0;
    ret += (150.0 * std::sin(_x / 12.0 * PI) + 300.0 * std::sin(_x / 30.0 * PI)) * 2.0 / 3.0;
    return ret;
}

// WGS84 转 GCJ-02
// 参数：lat 纬度，lon 经度
// 返回：GCJ-02 坐标
inline void wgs84_to_gcj02(const double _wlat, const double _wlon, double &_glat, double &_glon) 
{
    if (out_of_china(_wlat, _wlon)) 
    {
        // 中国境外不进行偏移
        _glat = _wlat;
        _glon = _wlon;
        return;
    }

    double dlat = transform_lat(_wlon - 105.0, _wlat - 35.0);
    double dlon = transform_lon(_wlon - 105.0, _wlat - 35.0);

    double rad_lat = _wlat / 180.0 * PI;
    double magic = std::sin(rad_lat);
    magic = 1 - EE * magic * magic;
    double sqrt_magic = std::sqrt(magic);

    dlat = (dlat * 180.0) / ((A * (1 - EE)) / (magic * sqrt_magic) * PI);
    dlon = (dlon * 180.0) / (A / sqrt_magic * std::cos(rad_lat) * PI);

    _glat = _wlat + dlat;
    _glon = _wlon + dlon;
}

#endif // __UTIL_H__
