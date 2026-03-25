#ifndef DATA_H
#define DATA_H

namespace Layer {
inline constexpr double LAYER_PLAYER_1 = 0;
inline constexpr double LAYER_PLAYER_2 = 1;
inline constexpr double LAYER_PLAYER_3 = 10;
}
namespace carMusicSysconfig {
inline constexpr const char * CACHEDATA_BASE = "cacheData"; // 缓存文件根目录名
inline constexpr const char * WEATHER_API_KEY = "f909afa4e64542abb2920be08d0f2995"; // 和风天气 API Key
inline constexpr const char * WEATHER_ICON_PATH = ":/Resource/weatherIcons/"; // 天气图标资源前缀路径
inline constexpr const char * WEATHER_APP_PATH = ":/Resource/app/"; // 应用资源路径
inline constexpr const char * WEATHER_BACKGROUND = "7.png"; // 天气模块背景图文件名
inline constexpr const char * ICON_USE_FILL = ""; // 天气图标后缀（空表示不使用 fill 版本）
inline constexpr const char * CACHE_WEATHER = "weather"; // 天气缓存键
inline constexpr const char * DEFAULT_CITY_ID = "101270101"; // 默认城市 ID（成都）
inline constexpr const char * CITY_LOOKUP_KEY_PREFIX = "city_lookup_"; // 城市查询缓存键前缀
inline constexpr const char * WEATHER_API_7D_URL = "https://n36cdphw5g.re.qweatherapi.com/v7/weather/7d?location=%1&key=%2"; // 7日天气查询接口模板
inline constexpr const char * WEATHER_CITY_LOOKUP_URL = "https://n36cdphw5g.re.qweatherapi.com/geo/v2/city/lookup?location=%1&key=%2"; // 城市名转城市ID接口模板
inline constexpr const char * REQUEST_TYPE_KEY = "requestType"; // QNetworkReply 属性键：请求类型
inline constexpr const char * REQUEST_TYPE_WEATHER = "weather"; // 请求类型值：天气请求
inline constexpr const char * REQUEST_TYPE_CITY_LOOKUP = "cityLookup"; // 请求类型值：城市查询请求
inline constexpr const char * REQUEST_CITY_ID_KEY = "cityId"; // QNetworkReply 属性键：城市ID
inline constexpr const char * REQUEST_CITY_NAME_KEY = "cityName"; // QNetworkReply 属性键：城市名
inline constexpr const char * API_CODE_OK = "200"; // 和风天气业务成功码
inline constexpr int HTTP_STATUS_OK = 200; // HTTP 成功状态码
inline constexpr const char * APP_WEATHER = "weather"; // 应用标识：天气
inline constexpr const char * APP_QQMUSIC = "QQMusic"; // 应用标识：QQ音乐
inline constexpr const char * APP_AMAP = "amap"; // 应用标识：高德地图
inline constexpr const char * APP_BILIBILI = "bilibili"; // 应用标识：哔哩哔哩
inline constexpr const char * OVERLAY_WEATHER = "weather"; // 图层名称：天气浮层
inline constexpr const char * OVERLAY_DESKTOP = "desktop"; // 图层名称：桌面浮层
inline constexpr const char * PROJECT_ROOT_NAME = "carMusicSys"; // 项目根目录名（用于路径截取）
inline constexpr const char * WALLPAPER_DIR_SUFFIX = "/Wallpaper/"; // 壁纸目录相对后缀
inline constexpr const char * APP_ICON_PATH = ":/Resource/app/5.jpg"; // 主窗口图标资源路径
inline constexpr int LABEL_SIZE_1 = 100; // UI 标签基础尺寸
inline constexpr int CACHELIMIT = 1000; // 图标缓存上限
inline constexpr int WEAATHER_TIMEOUT_H = 6; // 缓存有效时长（小时）
inline constexpr int WALLPAPER_SWITCH_INTERVAL_MS = 1000*1; // 壁纸切换周期（毫秒）
inline constexpr int CLOCK_TICK_INTERVAL_MS = 1000*60; // 时钟刷新周期（毫秒）
inline constexpr int TIME_CONTAINER_X = 280; // 时间容器初始 X 坐标
inline constexpr int TIME_CONTAINER_Y = 30; // 时间容器初始 Y 坐标
inline constexpr int TIME_CONTAINER_W = 200; // 时间容器初始宽度
inline constexpr int TIME_CONTAINER_H = 150; // 时间容器初始高度
inline constexpr int TIME_LAYOUT_SPACING = 10; // 时间布局垂直间距
inline constexpr const char * DEFAULT_CITY = "成都";//天气显示默认城市
}


#endif // DATA_H
