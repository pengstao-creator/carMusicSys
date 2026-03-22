#include "weather.h"
#include "ui_weather.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
weather::weather(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::weather)
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

void weather::getweatherForCity(const QString &cityName)
{
    // 请将此URL替换为你选择的天气API的真实地址
    // 注意：你需要先去相关网站（如和风天气、OpenweatherMap等）注册并获取你的API Key
    // 这里只是一个示例URL，实际使用时需要根据API文档构建正确的URL
    QString urlString = QString("https://devapi.qweather.com/v7/weather/7d?location=%1&key=%2")
                            .arg("101270101")
                            .arg("f909afa4e64542abb2920be08d0f2995");
    QUrl url(urlString);
    QNetworkRequest request(url);

    // 发送GET请求
    manager->get(request);
    qDebug() << "Sending request for city:" << cityName;
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

void weather::updateWeather(const QStringList &weekForecast)
{
    qDebug() << weekForecast;
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
   for (int i = 0; i <= dailyArray.size(); ++i) {
        QJsonObject dayObj = dailyArray[i].toObject();
        QString fxDate = dayObj.value("fxDate").toString();
        QString tempMax = dayObj.value("tempMax").toString();
        QString tempMin = dayObj.value("tempMin").toString();
        QString textDay = dayObj.value("textDay").toString();

        // 格式化字符串,"11月16日: 晴 13° / 0°"
        // 这里简单使用原始日期，你可以根据需要转换格式
        QString dayInfo = QString("%1: %2 %3° / %4°").arg(fxDate, textDay, tempMax, tempMin);
        weekForecast.append(dayInfo);
    }

    // 发出信号，传递数据到UI
    emit weatherDataReady(weekForecast);
}
