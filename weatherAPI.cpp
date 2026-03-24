#include "weatherAPI.h"
#include "CacheManager.hpp"
#include "Data.h"
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

weatherAPI::weatherAPI(QObject *parent)
    : QObject(parent)
    , cacheData(std::make_unique<CacheManager<WeatherData>>(
          carMusicSysconfig::CACHEDATA_BASE, carMusicSysconfig::WEAATHER_TIMEOUT_H))
{
    manager = new QNetworkAccessManager(this);
    // 连接管理器的finished信号到我们的处理槽函数
    connect(manager, &QNetworkAccessManager::finished, this, &weatherAPI::onReplyFinished);
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
        qDebug() << "toJson" << __FILE__;
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
        qDebug() << "fromJson" << __FILE__;
        return true;
    }
};

void weatherAPI::getweatherForCity(const QString &cityName)
{
    // 判断缓存数据是否符合条件
    WeatherData weatherCache;
    if (cacheData->load(carMusicSysconfig::CACHE_WEATHER, weatherCache))
    {
        parseweatherJson(weatherCache.toJson());
        return;
    }
    qDebug() << "开始API 请求";
    const QString location = cityName.trimmed().isEmpty() ? QStringLiteral("101270101") : cityName.trimmed();
    QString urlString = QString("https://n36cdphw5g.re.qweatherapi.com/v7/weather/7d?location=%1&key=%2")
                            .arg(location)
                            .arg(carMusicSysconfig::WEATHER_API_KEY);
    QUrl url(urlString);
    QNetworkRequest request(url);

    // 发送GET请求
    manager->get(request);
}

void weatherAPI::onReplyFinished(QNetworkReply *reply)
{
    // 检查网络错误和HTTP状态码
    if (reply->error() == QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 200) { // 请求成功
            QByteArray data = reply->readAll();
            qDebug() << "Received data:" << data;
            parseweatherJson(data); // 解析数据
        } else {
            qWarning() << "HTTP error, status code:" << statusCode;
        }
    } else {
        qWarning() << "Network error:" << reply->errorString();
    }
    reply->deleteLater(); // 释放reply对象
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
    if (code != "200") {
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
        QString amIcon = carMusicSysconfig::WEATHER_ICON_PATH + QString("%1%2.svg").arg(amcode).arg(carMusicSysconfig::ICON_USE_FILL);
        QString pmIcon = carMusicSysconfig::WEATHER_ICON_PATH + QString("%1%2.svg").arg(pmcode).arg(carMusicSysconfig::ICON_USE_FILL);

        // 构建天气信息数组
        QVector<QString> dayInfo({formattedDate, amIcon, amText, tempMax, pmIcon, pmText, tempMin});
        weekForecast.append(dayInfo);
    }
    WeatherData weatherCache;
    if (weatherCache.fromJson(jsonData))
    {
        cacheData->save(carMusicSysconfig::CACHE_WEATHER, weatherCache);
    }
    // 发出信号，传递数据到UI
    emit weatherDataReady(weekForecast);
}
