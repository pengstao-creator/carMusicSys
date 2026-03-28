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

protected:
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

    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QHBoxLayout *cardsLayout;
    QVector<QWidget*> dayCards;
    weatherAPI *weatherService;
    QString backgroundImagePath;
    QVector<QVector<QString>> latestForecast;
    QString currentCityName;
    QString pendingCityName;
    QLabel *errorTipLabel;
    QLabel *backgroundLabel;
    QPixmap backgroundPixmap;
    QTimer *errorTipTimer;
    bool rebuildPending;
    Ui::WeatherUi *ui;
};

#endif // WEATHERUI_H
