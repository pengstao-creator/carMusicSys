#ifndef DATA_H
#define DATA_H

namespace Layer {
static const double LAYER_PLAYER_1 = 0;
static const double LAYER_PLAYER_2 = 1;
static const double LAYER_PLAYER_3 = 10;
}
namespace carMusicSysconfig {
static const char * CACHEDATA_BASE = "cacheData";
static const char * WEATHER_API_KEY = "f909afa4e64542abb2920be08d0f2995";
static const char * WEATHER_ICON_PATH = ":/Resource/weatherIcons/";
static const char * WEATHER_APP_PATH = ":/Resource/app/";
static const char * WEATHER_BACKGROUND = "7.png";
static const char * ICON_USE_FILL = ""; // 为空就是使用 无填充
static const char * CACHE_WEATHER = "weather";
static const int LABEL_SIZE_1 = 100;
static const int CACHELIMIT = 1000; //图标缓存上限
}


#endif // DATA_H
