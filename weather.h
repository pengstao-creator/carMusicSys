#ifndef weather_H
#define weather_H

#include <QWidget>
#include "CacheManager.hpp"
#include <QString>
#include<memory>
class QNetworkAccessManager;
class QNetworkReply;
class QJsonDocument;
class QJsonObject;
class QJsonArray;
class QSvgRenderer;
class QLabel;
class QPixmap;
class QPainter;

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
    void displaySvgOnLabel(QLabel *label, const QString &svgPath, int size,int iconcode);
protected:
    void mousePressEvent(QMouseEvent *event) override {}
signals:
    void weatherDataReady(const QVector<QVector<QString>> &weekForecast); // 数据就绪信号
    void exit();
private slots:
    void onReplyFinished(QNetworkReply *reply); // 网络请求完成时的槽函数
    void updateWeather(const QVector<QVector<QString>> &weekForecast);

    void on_exit_clicked();

private:
    void parseweatherJson(const QByteArray &jsonData); // 解析JSON数据
    void testNetworkConnection(); // 测试网络连接
    class WeatherData;

    Ui::weather *ui;
    QNetworkAccessManager *manager;
    std::unique_ptr<CacheManager<WeatherData>> cacheData;
    QHash<int,QPixmap> cacheIcon;
    QVector<QWidget*> weatherCon;
};

#endif // weather_H
