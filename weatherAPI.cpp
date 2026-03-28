#include "weatherAPI.h"
#include "CacheManager.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLocale>
#include <QDate>
#include <QRegularExpression>
#include <QDir>
#include <QFileInfo>

namespace {
constexpr const char *kCacheDataBase = "cacheData";
constexpr const char *kWeatherApiKey = "f909afa4e64542abb2920be08d0f2995";
constexpr const char *kWeatherIconPath = ":/Resource/weatherIcons/";
constexpr const char *kIconUseFill = "";
constexpr const char *kDefaultCityIdRaw = "101270101";
constexpr const char *kCityLookupKeyPrefixRaw = "city_lookup_";
constexpr const char *kWeatherApi7dUrl = "https://n36cdphw5g.re.qweatherapi.com/v7/weather/7d?location=%1&key=%2";
constexpr const char *kWeatherCityLookupUrl = "https://n36cdphw5g.re.qweatherapi.com/geo/v2/city/lookup?location=%1&key=%2";
constexpr const char *kRequestTypeKey = "requestType";
constexpr const char *kRequestTypeWeather = "weather";
constexpr const char *kRequestTypeCityLookup = "cityLookup";
constexpr const char *kRequestCityIdKey = "cityId";
constexpr const char *kRequestCityNameKey = "cityName";
constexpr const char *kApiCodeOk = "200";
constexpr int kHttpStatusOk = 200;
constexpr int kWeatherTimeoutHours = 6;
const QString kDefaultCityId = QString::fromUtf8(kDefaultCityIdRaw);
const QString kCityLookupPrefix = QString::fromUtf8(kCityLookupKeyPrefixRaw);
}

weatherAPI::weatherAPI(QObject *parent)
    : QObject(parent)
    , cacheData(std::make_unique<CacheManager<WeatherData>>(
          kCacheDataBase, kWeatherTimeoutHours))
    , cityLookupCacheData(std::make_unique<CacheManager<CityLookupData>>(
          kCacheDataBase, kWeatherTimeoutHours))
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &weatherAPI::onReplyFinished);
    loadCityIdCache();
}

weatherAPI::~weatherAPI()
{
}

class weatherAPI::WeatherData
{
public:
    QString code;
    QString updateTime;
    QJsonArray daily;   // 存储daily数组，后续可直接使用

    QByteArray toJson() const
    {
        QJsonObject root;
        root["code"] = code;
        root["updateTime"] = updateTime;
        root["daily"] = daily;
        return QJsonDocument(root).toJson();
    }

    bool fromJson(const QByteArray &jsonData)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull()) return false;
        QJsonObject obj = doc.object();
        code = obj.value("code").toString();
        updateTime = obj.value("updateTime").toString();
        daily = obj.value("daily").toArray();
        return true;
    }
};

class weatherAPI::CityLookupData
{
public:
    QString cityName;
    QString cityId;

    QByteArray toJson() const
    {
        QJsonObject obj;
        obj["cityName"] = cityName;
        obj["cityId"] = cityId;
        return QJsonDocument(obj).toJson();
    }

    bool fromJson(const QByteArray &jsonData)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull()) return false;
        QJsonObject obj = doc.object();
        cityName = obj.value("cityName").toString();
        cityId = obj.value("cityId").toString();
        return !cityName.isEmpty() && !cityId.isEmpty();
    }
};

QString weatherAPI::cityLookupKey(const QString &cityName) const
{
    QString key = cityName.trimmed();
    key.replace(QRegularExpression("[\\\\/:*?\"<>|\\s]+"), "_");
    return kCityLookupPrefix + key;
}

void weatherAPI::loadCityIdCache()
{
    cityIdMap.clear();
    const QDir dir(cityLookupCacheData->getcacheDir());
    const QStringList files = dir.entryList(QStringList() << QString("%1*.json").arg(kCityLookupPrefix), QDir::Files);
    for (const QString &fileName : files) {
        QString key = QFileInfo(fileName).completeBaseName();
        CityLookupData data;
        if (cityLookupCacheData->load(key, data)) {
            cityIdMap.insert(data.cityName, data.cityId);
        }
    }
}

void weatherAPI::saveCityIdCache(const QString &cityName, const QString &resolvedCityId)
{
    CityLookupData data;
    data.cityName = cityName;
    data.cityId = resolvedCityId;
    cityLookupCacheData->save(cityLookupKey(cityName), data);
    cityIdMap.insert(cityName, resolvedCityId);
}

void weatherAPI::getweatherForCity(const QString &cityName)
{
    const QString input = cityName.trimmed();
    cityId.clear();
    if (input.isEmpty()) {
        cityId = kDefaultCityId;
    } else if (cityIdMap.contains(input)) {
        cityId = cityIdMap.value(input);
    }

    WeatherData weatherCache;
    if (!cityId.isEmpty() && cacheData->load(cityId, weatherCache))
    {
        parseweatherJson(weatherCache.toJson());
        return;
    }
    qDebug() << "开始API 请求";
    if (!cityId.isEmpty()) {
        QString urlString = QString(kWeatherApi7dUrl)
                                .arg(cityId)
                                .arg(kWeatherApiKey);
        QNetworkRequest request{QUrl(urlString)};
        QNetworkReply *reply = manager->get(request);
        reply->setProperty(kRequestTypeKey, kRequestTypeWeather);
        reply->setProperty(kRequestCityIdKey, cityId);
        return;
    }
    getCityId(input);
}

void weatherAPI::getCityId(const QString &cityName)
{
    QString urlString = QString(kWeatherCityLookupUrl)
                            .arg(cityName)
                            .arg(kWeatherApiKey);
    QNetworkRequest request{QUrl(urlString)};
    QNetworkReply *reply = manager->get(request);
    reply->setProperty(kRequestTypeKey, kRequestTypeCityLookup);
    reply->setProperty(kRequestCityNameKey, cityName);
}

void weatherAPI::onReplyFinished(QNetworkReply *reply)
{
    if (reply->property(kRequestTypeKey).toString() == kRequestTypeCityLookup) {
        onCitySearchFinished(reply);
        return;
    }
    if (reply->error() == QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == kHttpStatusOk) {
            QByteArray data = reply->readAll();
            cityId = reply->property(kRequestCityIdKey).toString();
            qDebug() << "Received data:" << data;
            parseweatherJson(data);
        } else {
            emit requestFailed("请求天气服务失败");
        }
    } else {
        emit requestFailed("请求天气服务失败");
    }
    reply->deleteLater();
}

void weatherAPI::onCitySearchFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        if (obj.value("code").toString() == kApiCodeOk) {
            QJsonArray locationArray = obj.value("location").toArray();
            if (!locationArray.isEmpty()) {
                QString queriedCityId = locationArray[0].toObject().value("id").toString();
                if (!queriedCityId.isEmpty()) {
                    cityId = queriedCityId;
                    const QString cityName = reply->property(kRequestCityNameKey).toString();
                    saveCityIdCache(cityName, cityId);
                    QString weatherUrl = QString(kWeatherApi7dUrl)
                                             .arg(cityId)
                                             .arg(kWeatherApiKey);
                    QNetworkRequest request{QUrl(weatherUrl)};
                    QNetworkReply *weatherReply = manager->get(request);
                    weatherReply->setProperty(kRequestTypeKey, kRequestTypeWeather);
                    weatherReply->setProperty(kRequestCityIdKey, cityId);
                }
            }
        }
    } else {
        emit requestFailed("城市ID获取失败");
    }
    reply->deleteLater();
}

void weatherAPI::parseweatherJson(const QByteArray &jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "Failed to parse JSON!";
        return;
    }

    QJsonObject rootObj = doc.object();

    // 检查返回码，200表示成功
    QString code = rootObj.value("code").toString();
    if (code != kApiCodeOk) {
        qWarning() << "API returned error code:" << code;
        return;
    }

    // 获取daily数组
    QJsonArray dailyArray = rootObj.value("daily").toArray();
    if (dailyArray.isEmpty()) {
        qWarning() << "No daily forecast data!";
        return;
    }

    // 解析未来的数据
    QVector<QVector<QString>> weekForecast;
    for (int i = 0; i < dailyArray.size(); ++i) {
        QJsonObject dayObj = dailyArray[i].toObject();
        QString amcode = dayObj.value("iconDay").toString();
        QString amText = dayObj.value("textDay").toString();
        QString tempMax = dayObj.value("tempMax").toString();
        QString pmcode = dayObj.value("iconNight").toString();
        QString pmText = dayObj.value("textNight").toString();
        QString tempMin = dayObj.value("tempMin").toString();
        QString fxDate = dayObj.value("fxDate").toString();

        // 格式化日期，例如"周一 3/24"
        QDate date = QDate::fromString(fxDate, "yyyy-MM-dd");
        QString weekDay = QLocale(QLocale::Chinese).dayName(date.dayOfWeek(), QLocale::ShortFormat);
        QString formattedDate = QString("%1 %2/%3").arg(weekDay).arg(date.month()).arg(date.day());

        // 构建图标路径
        QString amIcon = QString(kWeatherIconPath) + QString("%1%2.svg").arg(amcode).arg(kIconUseFill);
        QString pmIcon = QString(kWeatherIconPath) + QString("%1%2.svg").arg(pmcode).arg(kIconUseFill);

        // 构建天气信息数组
        QVector<QString> dayInfo({formattedDate, amIcon, amText, tempMax, pmIcon, pmText, tempMin});
        weekForecast.append(dayInfo);
    }
    WeatherData weatherCache;
    if (weatherCache.fromJson(jsonData))
    {
        const QString weatherKey = cityId.isEmpty() ? kDefaultCityId : cityId;
        cacheData->save(weatherKey, weatherCache);
    }
    emit weatherDataReady(weekForecast);
}
