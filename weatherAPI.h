#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QHash>
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
    void requestFailed(const QString& strerr);
private slots:
    void onReplyFinished(QNetworkReply *reply); // 网络请求完成时的槽函数
    void onCitySearchFinished(QNetworkReply *reply); // 城市查询接口返回时的处理

private:
    void parseweatherJson(const QByteArray &jsonData); // 解析JSON数据
    void getCityId(const QString &cityName); // 通过城市名查询和风城市ID
    void loadCityIdCache();
    void saveCityIdCache(const QString &cityName, const QString &resolvedCityId);
    QString cityLookupKey(const QString &cityName) const;
    void testNetworkConnection(); // 测试网络连接
    class WeatherData;
    class CityLookupData;

    QNetworkAccessManager *manager;
    std::unique_ptr<CacheManager<WeatherData>> cacheData;
    std::unique_ptr<CacheManager<CityLookupData>> cityLookupCacheData;
    QString cityId; // 最近一次查询到的城市ID
    QHash<QString, QString> cityIdMap;
};

#endif // WEATHERAPI_H
