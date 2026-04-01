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
#include <QFutureWatcher>
#include <QtConcurrent>
#include "AppThreadPool.h"

namespace {
constexpr const char *kCacheDataBase = "cacheData";
constexpr const char *kWeatherApiKey = "f909afa4e64542abb2920be08d0f2995";
constexpr const char *kWeatherIconPath = ":/Resource/weatherIcons/";
constexpr const char *kIconUseFill = "-fill";
constexpr const char *kDefaultCityIdRaw = "101270101";
constexpr const char *kWeatherApi7dUrl = "https://n36cdphw5g.re.qweatherapi.com/v7/weather/7d?location=%1&key=%2";
constexpr const char *kWeatherCityLookupUrl = "https://n36cdphw5g.re.qweatherapi.com/geo/v2/city/lookup?location=%1&key=%2";
constexpr const char *kRequestTypeKey = "requestType";
constexpr const char *kRequestTypeWeather = "weather";
constexpr const char *kRequestTypeCityLookup = "cityLookup";
constexpr const char *kRequestCityIdKey = "cityId";
constexpr const char *kRequestCityNameKey = "cityName";
constexpr const char *kRequestSeqKey = "requestSeq";
constexpr const char *kApiCodeOk = "200";
constexpr int kHttpStatusOk = 200;
constexpr int kWeatherTimeoutHours = 6;
const QString kDefaultCityId = QString::fromUtf8(kDefaultCityIdRaw);
const QString kCityLookup= QString::fromUtf8(kRequestTypeCityLookup);
}

weatherAPI::weatherAPI(QObject *parent)
    : QObject(parent)
    , cacheData(std::make_unique<CacheManager<WeatherData>>(
          kCacheDataBase, kWeatherTimeoutHours))
    , cityLookupCacheData(std::make_unique<CacheManager<CityLookupData>>(
          kCacheDataBase, kWeatherTimeoutHours))
    , cityData(std::make_unique<CityLookupData>())
    , activeRequestSeq(0)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &weatherAPI::onReplyFinished);
    loadCityIdCache();
}

weatherAPI::~weatherAPI()
{
    //析构保存缓存
    saveCityIdCache();
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
    QByteArray toJson() const
    {
        if(cityIdMap.isEmpty())return QByteArray();
        QJsonObject obj;
        for(auto it = cityIdMap.begin();it != cityIdMap.end();++it)
        {
            obj[it.key()] = it.value();
        }
        return QJsonDocument(obj).toJson();
    }

    QString find(const QString& key) const
    {
        if(cityIdMap.isEmpty()) return QString();
        auto it = cityIdMap.find(key);
        if(it != cityIdMap.end())
        {
            return it.value();
        }
        return QString();
    }


    void addDate(const QString& key,const QString& name)
    {
        cityIdMap.insert(key,name);
    }

    bool fromJson(const QByteArray &jsonData)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull()) return false;
        QJsonObject obj = doc.object();
        if(obj.isEmpty()) return false;
        for(auto it = obj.begin();it != obj.end();++it)
        {
            cityIdMap[it.key()] = it.value().toString();
        }
        return true;
    }
private:
    QHash<QString, QString> cityIdMap;
};

void weatherAPI::loadCityIdCache()
{
    // 防御式保护：cityData 为空时不进行解引用，避免启动阶段崩溃。
    if (!cityData) {
        return;
    }
    cityLookupCacheData->load(kCityLookup, *cityData);
}

void weatherAPI::saveCityIdCache()
{
    // 防御式保护：析构阶段若 cityData 异常为空，跳过保存，避免崩溃。
    if (!cityData) {
        return;
    }
    cityLookupCacheData->save(kCityLookup,*cityData);
}

void weatherAPI::getweatherForCity(const QString &cityName)
{
    const quint64 requestSeq = ++activeRequestSeq;
    const QString input = cityName.trimmed();
    cityId.clear();
    if (input.isEmpty()) {
        cityId = kDefaultCityId;
    } else{
        // 使用 trim 后的输入参与缓存查询，避免前后空格导致缓存 miss。
        cityId = cityData ? cityData->find(input) : QString();
    }

    if (!cityId.isEmpty()) {
        // 先在线程池中异步查缓存，未命中再请求网络，避免主线程被文件IO阻塞。
        tryLoadWeatherCacheAsync(cityId, requestSeq);
        return;
    }
    qDebug() << "开始API 请求";
    getCityId(input, requestSeq);
}

void weatherAPI::requestWeatherByCityId(const QString &targetCityId, quint64 requestSeq)
{
    QString urlString = QString(kWeatherApi7dUrl)
                            .arg(targetCityId)
                            .arg(kWeatherApiKey);
    QNetworkRequest request{QUrl(urlString)};
    QNetworkReply *reply = manager->get(request);
    reply->setProperty(kRequestTypeKey, kRequestTypeWeather);
    reply->setProperty(kRequestCityIdKey, targetCityId);
    reply->setProperty(kRequestSeqKey, QVariant::fromValue<qulonglong>(requestSeq));
}

void weatherAPI::tryLoadWeatherCacheAsync(const QString &targetCityId, quint64 requestSeq)
{
    auto *watcher = new QFutureWatcher<QByteArray>(this);
    auto *cache = cacheData.get();
    watcher->setFuture(QtConcurrent::run(AppThreadPool::globalPool(), [cache, targetCityId]() {
        WeatherData weatherCache;
        if (cache && cache->load(targetCityId, weatherCache)) {
            return weatherCache.toJson();
        }
        return QByteArray();
    }));

    connect(watcher, &QFutureWatcher<QByteArray>::finished, this, [this, watcher, targetCityId, requestSeq]() {
        const QByteArray cacheJson = watcher->result();
        watcher->deleteLater();
        if (requestSeq != activeRequestSeq) {
            return;
        }
        if (!cacheJson.isEmpty()) {
            cityId = targetCityId;
            parseweatherJson(cacheJson);
            return;
        }
        requestWeatherByCityId(targetCityId, requestSeq);
    });
}

void weatherAPI::getCityId(const QString &cityName, quint64 requestSeq)
{
    QString urlString = QString(kWeatherCityLookupUrl)
                            .arg(cityName)
                            .arg(kWeatherApiKey);
    QNetworkRequest request{QUrl(urlString)};
    QNetworkReply *reply = manager->get(request);
    reply->setProperty(kRequestTypeKey, kRequestTypeCityLookup);
    reply->setProperty(kRequestCityNameKey, cityName);
    reply->setProperty(kRequestSeqKey, QVariant::fromValue<qulonglong>(requestSeq));
}

void weatherAPI::onReplyFinished(QNetworkReply *reply)
{
    const quint64 requestSeq = reply->property(kRequestSeqKey).toULongLong();
    if (requestSeq != 0 && requestSeq != activeRequestSeq) {
        reply->deleteLater();
        return;
    }
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
    const quint64 requestSeq = reply->property(kRequestSeqKey).toULongLong();
    if (requestSeq != 0 && requestSeq != activeRequestSeq) {
        reply->deleteLater();
        return;
    }
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
                    if (cityData) {
                        cityData->addDate(cityName, cityId);
                    }
                    QString weatherUrl = QString(kWeatherApi7dUrl)
                                             .arg(cityId)
                                             .arg(kWeatherApiKey);
                    QNetworkRequest request{QUrl(weatherUrl)};
                    QNetworkReply *weatherReply = manager->get(request);
                    weatherReply->setProperty(kRequestTypeKey, kRequestTypeWeather);
                    weatherReply->setProperty(kRequestCityIdKey, cityId);
                    weatherReply->setProperty(kRequestSeqKey, QVariant::fromValue<qulonglong>(requestSeq));
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
        // 缓存写入走全局线程池，避免主线程在磁盘写入时出现卡顿。
        auto weatherCacheCopy = weatherCache;
        auto *cache = cacheData.get();
        QtConcurrent::run(AppThreadPool::globalPool(), [cache, weatherKey, weatherCacheCopy]() mutable {
            if (cache) {
                cache->save(weatherKey, weatherCacheCopy);
            }
        });
    }
    emit weatherDataReady(weekForecast);
}
