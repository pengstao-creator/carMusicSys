#ifndef WEATHERUI_H
#define WEATHERUI_H

#include "softwareuibase.h"
#include <QVector>
#include <QString>
#include <QPixmap>
class QScrollArea;
class QHBoxLayout;
class QLabel;
class QTimer;
class weatherAPI;
class QPaintEvent;
class QMouseEvent;
namespace Ui {
class WeatherUi;
}

class WeatherUi : public softwareUiBase
{
    Q_OBJECT
public:
    explicit WeatherUi(QWidget *parent = nullptr);
    ~WeatherUi();
    static const QString &getSoftname();
    static const QPixmap &getSofticon() ;
    static softwareUiBase*getSingleton() ;
    void updateWeather(const QVector<QVector<QString>> &weekForecast);
    void setBackground(const QString &backgroundPath);
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_exitButton_clicked();

    void on_searchButton_clicked();

    void on_confirmSearchButton_clicked();

private:
    void setupUI();
    void rebuildWeatherCards();
    void scheduleRebuildCards();
    void setSvgLabelIcon(QLabel *label, const QString &iconPath, int iconSize);
    QScrollArea *scrollArea;                  // 天气卡片横向滚动区域
    QWidget *scrollContent;                   // 滚动区域内容容器
    QHBoxLayout *cardsLayout;                 // 天气卡片横向布局
    QVector<QWidget*> dayCards;               // 7天卡片控件集合
    weatherAPI *weatherService;               // 天气服务对象（负责网络请求与数据解析）
    QString backgroundImagePath;              // 当前背景图片路径
    QVector<QVector<QString>> latestForecast; // 最近一次天气预报数据缓存
    QString currentCityName;                  // 当前展示城市名
    QString pendingCityName;                  // 待切换城市名（请求成功后再提交）
    QLabel *errorTipLabel;                    // 错误提示标签
    QPixmap backgroundPixmap;                 // 背景图片缓存
    QTimer *errorTipTimer;                    // 错误提示自动隐藏定时器
    bool rebuildPending;                      // 是否已有卡片重建任务在排队
    Ui::WeatherUi *ui;
};

#endif // WEATHERUI_H
