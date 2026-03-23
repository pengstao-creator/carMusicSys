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
#include <QSvgRenderer>
#include <QLabel>
#include <QPixmap>
#include <QPainter>


weather::weather(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::weather)
    , cacheData(std::make_unique<CacheManager<WeatherData>>(carMusicSysconfig::CACHEDATA_BASE,23))


{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    // 连接管理器的finished信号到我们的处理槽函数
    connect(manager, &QNetworkAccessManager::finished, this, &weather::onReplyFinished);
    connect(this,&weather::weatherDataReady,this,&weather::updateWeather);

    weatherCon = QVector<QWidget*>({
        ui->weather_1,
        ui->weather_2,
        ui->weather_3,
        ui->weather_4,
        ui->weather_5,
        ui->weather_6,
        ui->weather_7,
        ui->weather_8
    });

    // 将容器背景设置为透明
     for(auto wC : weatherCon)
    {
        // // 设置透明背景
        wC->setAttribute(Qt::WA_TranslucentBackground);
        wC->setStyleSheet("background: transparent;");
    }
    // 设置天气壁纸背景样式
    // 创建主窗口
    QString background= QString("#mainWidget {border-image: url(%1%2);}")
                        .arg(carMusicSysconfig::WEATHER_APP_PATH)
                        .arg(carMusicSysconfig::WEATHER_BACKGROUND);
    qDebug() << background;
    setObjectName("mainWidget");
    setStyleSheet(background);
   

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

void weather::updateWeather(const QVector<QVector<QString>> &weekForecast)
{
    if(weekForecast.isEmpty())return;



    int w_size = qMin(weekForecast.size(),7);
    for(int i =0;i <w_size;i++)
    {

        QString amcode = weekForecast[i][0];
        QString amText = weekForecast[i][1];
        QString tempMax = weekForecast[i][2];
        QString pmcode = weekForecast[i][3];
        QString pmText = weekForecast[i][4];
        QString tempMin = weekForecast[i][5];
        QString fxDate = weekForecast[i][6];
        QString amWeather(carMusicSysconfig::WEATHER_ICON_PATH \
        + QString("%1%2.svg").arg(amcode).arg(carMusicSysconfig::ICON_USE_FILL));
        QString pmWeather(carMusicSysconfig::WEATHER_ICON_PATH \
        + QString("%1%2.svg").arg(pmcode).arg(carMusicSysconfig::ICON_USE_FILL));
        QString am = QString("am_%1").arg(i+1);
        QString am_c = QString("am_c_%1").arg(i+1);
        QString pm = QString("pm_%1").arg(i+1);
        QString pm_c = QString("pm_c_%1").arg(i+1);
        QString date = QString("date_%1").arg(i+1);

        if(auto qam = weatherCon[i]->findChild<QLabel*>(am))
        {
            if(cacheIcon.find(amcode.toInt()) == cacheIcon.end())
            {
                displaySvgOnLabel(qam,amWeather,carMusicSysconfig::LABEL_SIZE_1,amcode.toInt());
            }
            else
            {
                qam->setPixmap(cacheIcon.value(amcode.toInt()));
            }
        }

        if(auto qam_c = weatherCon[i]->findChild<QLabel*>(am_c))
        {
            qam_c->setText(amText + "," + tempMax);
        }
        if(auto qpm = weatherCon[i]->findChild<QLabel*>(pm))
        {
            if(cacheIcon.find(pmcode.toInt()) == cacheIcon.end())
            {
                displaySvgOnLabel(qpm,pmWeather,carMusicSysconfig::LABEL_SIZE_1,pmcode.toInt());
            }
            else
            {
                qpm->setPixmap(cacheIcon.value(pmcode.toInt()));
            }
        }
        if(auto qpm_c = weatherCon[i]->findChild<QLabel*>(pm_c))
        {
            qpm_c->setText(pmText + "," + tempMin);
        }
        if(auto qdate = weatherCon[i]->findChild<QLabel*>(date))
        {
            qdate->setText(fxDate);
        }
        // weatherCon[i]->setStyleSheet("background-color: rgb(167, 240, 255);");
    }

}

void weather::displaySvgOnLabel(QLabel *label, const QString &svgPath, int size,int iconcode)
{
    QSvgRenderer renderer(svgPath);
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);      // 抗锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform); // 平滑变换
    renderer.render(&painter);
    label->setPixmap(pixmap);
    label->setFixedSize(size, size);
    if(cacheIcon.size() < carMusicSysconfig::CACHELIMIT)
    {
        cacheIcon.insert(iconcode,pixmap);
    }
}



void weather::on_exit_clicked()
{
    //暂时隐藏窗口
    close();
    //手动释放
    emit exit();

}

