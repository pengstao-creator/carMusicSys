#include "weather.h"
#include "ui_weather.h"
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
#include <QLabel>
weather::weather(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::weather)
    , cacheData(new CacheManager<WeatherData>(carMusicSysconfig::CACHEDATA_BASE,23))
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    // 连接管理器的finished信号到我们的处理槽函数
    connect(manager, &QNetworkAccessManager::finished, this, &weather::onReplyFinished);
    connect(this,&weather::weatherDataReady,this,&weather::updateWeather);
}

weather::~weather()
{
    delete ui;
}



class weather::WeatherData
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

void weather::getweatherForCity(const QString &cityName)
{

   //判断缓存数据是否符合条件
    WeatherData weatherCache;
    if(cacheData->load(carMusicSysconfig::CACHE_WEATHER,weatherCache))
    {
        parseweatherJson(weatherCache.toJson());
        return;
    }
    qDebug() << "开始API 请求";
    QString urlString = QString("https://n36cdphw5g.re.qweatherapi.com/v7/weather/7d?location=%1&key=%2")
                            .arg("101270101")
                            .arg(carMusicSysconfig::WEATHER_API_KEY);
    QUrl url(urlString);
    QNetworkRequest request(url);

    // 发送GET请求
    manager->get(request);
}

void weather::onReplyFinished(QNetworkReply *reply)
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
void weather::parseweatherJson(const QByteArray &jsonData)
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
    QStringList weekForecast;
    for (int i = 0; i < dailyArray.size(); ++i) {
        QJsonObject dayObj = dailyArray[i].toObject();
        QString amcode = dayObj.value("iconDay ").toString();
        QString amText = dayObj.value("textDay").toString();
        QString tempMax = dayObj.value("tempMax").toString();
        QString pmcode = dayObj.value("iconNight  ").toString();
        QString pmText = dayObj.value("textNight  ").toString();
        QString tempMin = dayObj.value("tempMin").toString();
        QString fxDate = dayObj.value("fxDate ").toString();

        // 格式化字符串,"11月16日,晴,13°,0°"
        // 这里简单使用原始日期，你可以根据需要转换格式
        QVector<QString> dayInfo({amcode,amText,tempMax,pmcode,pmText,tempMin,fxDate});
        weekForecast.append(dayInfo);
    }
    WeatherData weatherCache;
    if(weatherCache.fromJson(jsonData))
    {
        cacheData->save(carMusicSysconfig::CACHE_WEATHER,weatherCache);
    }
    // 发出信号，传递数据到UI
    emit weatherDataReady(weekForecast);
}

void weather::updateWeather(const QStringList &weekForecast)
{
    if(weekForecast.isEmpty())return;
    QVector<QWidget*> weatherCon{
        ui->weather_1,
        ui->weather_2,
        ui->weather_3,
        ui->weather_4,
        ui->weather_5,
        ui->weather_6,
        ui->weather_7,
        };

    int w_size = qMin(weekForecast.size() / weekForecast[0].size(),7);
    for(int i =0;i <w_size;i++)
    {
        qDebug() << w_size  << "weekForecast" << weekForecast.size();
        // QString amcode = weekForecast[i][0];
        // QString amText = weekForecast[i][1];
        // QString tempMax = weekForecast[i][2];
        // QString pmcode = weekForecast[i][3];
        // QString pmText = weekForecast[i][4];
        // QString tempMin = weekForecast[i][5];
        // QString fxDate = weekForecast[i][6];
        // QPixmap amWeather(QString("%1%2.svg").arg(amcode).arg(carMusicSysconfig::ICON_USE_FILL));
        // QPixmap pmWeather(QString("%1%2.svg").arg(pmcode).arg(carMusicSysconfig::ICON_USE_FILL));
        // QString am = QString("am_%1").arg(i+1);
        // QString am_c = QString("am_c%1").arg(i+1);
        // QString pm = QString("pm%1").arg(i+1);
        // QString pm_c = QString("pm_c%1").arg(i+1);
        // QString date = QString("date_%1").arg(i+1);
        // weatherCon[i]->findChild<QLabel*>(am)->setPixmap(amWeather);
        // weatherCon[i]->findChild<QLabel*>(am_c)->setText(amText + "," + tempMax);
        // weatherCon[i]->findChild<QLabel*>(pm)->setPixmap(pmWeather);
        // weatherCon[i]->findChild<QLabel*>(pm_c)->setText(pmText + "," + tempMin);
        // weatherCon[i]->findChild<QLabel*>(date)->setText(fxDate);
    }

}



