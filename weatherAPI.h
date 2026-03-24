#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <memory>

class QNetworkAccessManager;
class QNetworkReply;
template<typename DataType>
class CacheManager;

class weatherAPI : public QObject
{
    Q_OBJECT
public:
    explicit weatherAPI(QObject *parent = nullptr);
    ~weatherAPI();
    void getweatherForCity(const QString &cityName); // 根据城市名获取天气

signals:
    void weatherDataReady(const QVector<QVector<QString>> &weekForecast); // 数据就绪信号

private slots:
    void onReplyFinished(QNetworkReply *reply); // 网络请求完成时的槽函数

private:
    void parseweatherJson(const QByteArray &jsonData); // 解析JSON数据
    void testNetworkConnection(); // 测试网络连接
    class WeatherData;

    QNetworkAccessManager *manager;
    std::unique_ptr<CacheManager<WeatherData>> cacheData;
};

#endif // WEATHERAPI_H
