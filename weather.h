#ifndef weather_H
#define weather_H

#include <QWidget>
class QNetworkAccessManager;
class QNetworkReply;
class QJsonDocument;
class QJsonObject;
class QJsonArray;
namespace Ui {
class weather;
}

class weather : public QWidget
{
    Q_OBJECT
public:
    explicit weather(QWidget *parent = nullptr);
    ~weather();
    void getweatherForCity(const QString &cityName); // 根据城市名获取天气

signals:
    void weatherDataReady(const QStringList &weekForecast); // 数据就绪信号

private slots:
    void onReplyFinished(QNetworkReply *reply); // 网络请求完成时的槽函数
    void updateWeather(const QStringList &weekForecast);

private:
    void parseweatherJson(const QByteArray &jsonData); // 解析JSON数据
    void testNetworkConnection(); // 测试网络连接

    Ui::weather *ui;
    QNetworkAccessManager *manager;
};

#endif // weather_H
