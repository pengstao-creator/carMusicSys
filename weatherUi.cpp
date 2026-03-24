#include "weatherUi.h"
#include "ui_weatherUi.h"
#include "Data.h"
#include "weatherAPI.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSvgRenderer>
#include <QtGlobal>
#include <QVBoxLayout>

namespace {
constexpr int kBaseWindowWidth = 800;
constexpr int kBaseWindowHeight = 400;
constexpr int kCardWidth = 160;
constexpr int kCardSpacing = 15;
constexpr int kCardMargin = 16;
constexpr int kCardInnerSpacing = 12;
constexpr int kIconSize = 32;
constexpr int kAmPmSpacing = 8;
constexpr int kDateFontSize = 14;
constexpr int kTextFontSize = 12;

QLabel *createSvgLabel(const QString &iconPath, QWidget *parent, int iconSize)
{
    QLabel *label = new QLabel(parent);
    label->setFixedSize(iconSize, iconSize);

    QPixmap pixmap(iconSize, iconSize);
    pixmap.fill(Qt::transparent);

    QSvgRenderer renderer(iconPath);
    if (renderer.isValid()) {
        QPainter painter(&pixmap);
        renderer.render(&painter);
        label->setPixmap(pixmap);
    }

    return label;
}
}

WeatherUi::WeatherUi(QWidget *parent) : QWidget(parent)
    , weatherService(new weatherAPI(this))
    , ui(new Ui::WeatherUi)
{
    setupUI();
    const QString backgroundPath = QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH)
                                 + QString::fromUtf8(carMusicSysconfig::WEATHER_BACKGROUND);
    setBackground(backgroundPath);

    connect(weatherService, &weatherAPI::weatherDataReady, this, &WeatherUi::updateWeather);
    weatherService->getweatherForCity(QStringLiteral("成都"));
}

WeatherUi::~WeatherUi()
{
    delete ui;
}

void WeatherUi::setupUI()
{
    // 初始化UI布局
    ui->setupUi(this);
    resize(kBaseWindowWidth, kBaseWindowHeight);
    // 设置窗口对象名，用于样式表
    setObjectName("weatherMainWidget");

    // 获取UI中的滚动区域、内容容器和卡片布局
    scrollArea = ui->weatherScrollArea;
    scrollContent = ui->weatherScrollContent;
    cardsLayout = ui->cardsLayout;

    // 配置滚动区域
    if (scrollArea) {
        // 设置滚动区域内容可调整大小
        scrollArea->setWidgetResizable(true);
        // 设置水平滚动条按需显示
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        // 禁用垂直滚动条
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    
    
    // 初始化搜索面板为隐藏状态
    if (ui->searchPanel) {
        ui->searchPanel->setVisible(false);
    }
    
    // 清理 UI 中用于设计预览的示例背景和文字
    setStyleSheet(QString());
    const QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel *label : labels) {
        label->clear();
    }
    // 设置默认城市为成都市
    if (ui->cityWeatherLabel) {
        ui->cityWeatherLabel->setText(QStringLiteral("成都市天气"));
    }

    // 清空 UI 中示例卡片，运行时由代码动态创建
    if (cardsLayout) {
        while (cardsLayout->count() > 0) {
            // 移除布局中的第一个项目
            QLayoutItem *item = cardsLayout->takeAt(0);
            if (!item) {
                continue;
            }
            // 如果项目包含widget，删除它
            if (QWidget *w = item->widget()) {
                delete w;
            }
            // 删除布局项目本身
            delete item;
        }
    }
}

void WeatherUi::createDayCard(const QString &date, const QString &amIcon, const QString &amText, 
                             const QString &pmIcon, const QString &pmText)
{
    const qreal scale = qMin(static_cast<qreal>(width()) / kBaseWindowWidth,
                             static_cast<qreal>(height()) / kBaseWindowHeight);
    const qreal effectiveScale = qMax<qreal>(0.6, scale);
    const int cardWidth = qMax(120, qRound(kCardWidth * effectiveScale));
    const int cardMargin = qMax(8, qRound(kCardMargin * effectiveScale));
    const int cardInnerSpacing = qMax(6, qRound(kCardInnerSpacing * effectiveScale));
    const int iconSize = qMax(18, qRound(kIconSize * effectiveScale));
    const int amPmSpacing = qMax(4, qRound(kAmPmSpacing * effectiveScale));
    const int dateFontSize = qMax(10, qRound(kDateFontSize * effectiveScale));
    const int textFontSize = qMax(9, qRound(kTextFontSize * effectiveScale));

    // 创建卡片容器
    QWidget *card = new QWidget(scrollContent);
    card->setObjectName("weatherDayCard");
    card->setFixedWidth(cardWidth);
    card->setStyleSheet(
        "QWidget#weatherDayCard { "
        "    background: rgba(255, 255, 255, 0.30); "
        "    border-radius: 14px; "
        "}"
    );

    // 创建卡片内部布局
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(cardMargin, cardMargin, cardMargin, cardMargin);
    cardLayout->setSpacing(cardInnerSpacing);

    // 日期标签
    QLabel *dateLabel = new QLabel(date, card);
    dateLabel->setObjectName("weatherDateLabel");
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setStyleSheet(QString("QLabel#weatherDateLabel { color: white; font-size: %1px; font-weight: bold; }")
                                 .arg(dateFontSize));
    cardLayout->addWidget(dateLabel);

    // 早上天气
    QWidget *amWidget = new QWidget(card);
    amWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *amLayout = new QHBoxLayout(amWidget);
    amLayout->setContentsMargins(0, 0, 0, 0);
    amLayout->setSpacing(amPmSpacing);

    QLabel *amSvg = createSvgLabel(amIcon, amWidget, iconSize);
    QLabel *amLabel = new QLabel(amText, amWidget);
    amLabel->setStyleSheet(QString("color: white; font-size: %1px;").arg(textFontSize));

    amLayout->addWidget(amSvg);
    amLayout->addWidget(amLabel);
    cardLayout->addWidget(amWidget);

    // 晚上天气
    QWidget *pmWidget = new QWidget(card);
    pmWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *pmLayout = new QHBoxLayout(pmWidget);
    pmLayout->setContentsMargins(0, 0, 0, 0);
    pmLayout->setSpacing(amPmSpacing);

    QLabel *pmSvg = createSvgLabel(pmIcon, pmWidget, iconSize);
    QLabel *pmLabel = new QLabel(pmText, pmWidget);
    pmLabel->setStyleSheet(QString("color: white; font-size: %1px;").arg(textFontSize));

    pmLayout->addWidget(pmSvg);
    pmLayout->addWidget(pmLabel);
    cardLayout->addWidget(pmWidget);

    cardsLayout->addWidget(card);
    dayCards.append(card);
}

void WeatherUi::updateWeather(const QVector<QVector<QString>> &weekForecast)
{
    latestForecast = weekForecast;
    rebuildWeatherCards();
}

void WeatherUi::rebuildWeatherCards()
{
    for (QWidget *card : dayCards) {
        cardsLayout->removeWidget(card);
        delete card;
    }
    dayCards.clear();

    for (const QVector<QString> &dayData : latestForecast) {
        if (dayData.size() >= 7) {
            QString date = dayData[0];
            QString amIcon = dayData[1];
            QString amText = dayData[2] + " " + dayData[3] + "°C";
            QString pmIcon = dayData[4];
            QString pmText = dayData[5] + " " + dayData[6] + "°C";
            
            createDayCard(date, amIcon, amText, pmIcon, pmText);
        }
    }

    const qreal scale = qMin(static_cast<qreal>(width()) / kBaseWindowWidth,
                             static_cast<qreal>(height()) / kBaseWindowHeight);
    const qreal effectiveScale = qMax<qreal>(0.6, scale);
    const int scaledCardWidth = qMax(120, qRound(kCardWidth * effectiveScale));
    const int scaledCardSpacing = qMax(6, qRound(kCardSpacing * effectiveScale));
    const int cardCount = dayCards.size();
    const int contentWidth = (cardCount * scaledCardWidth) + (qMax(0, cardCount - 1) * scaledCardSpacing);
    cardsLayout->setSpacing(scaledCardSpacing);
    scrollContent->setMinimumWidth(contentWidth);
    scrollContent->adjustSize();
}

void WeatherUi::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!latestForecast.isEmpty()) {
        rebuildWeatherCards();
    }
}

void WeatherUi::setBackground(const QString &backgroundPath)
{
    backgroundImagePath = backgroundPath;
    update();
}

void WeatherUi::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (!backgroundImagePath.isEmpty()) {
        QPixmap bg(backgroundImagePath);
        if (!bg.isNull()) {
            const QPixmap scaled = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            const int x = (scaled.width() - width()) / 2;
            const int y = (scaled.height() - height()) / 2;
            painter.drawPixmap(rect(), scaled, QRect(x, y, width(), height()));
        }
    }

}

void WeatherUi::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void WeatherUi::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
}

void WeatherUi::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

void WeatherUi::on_exitButton_clicked()
{
    close();
    emit exit();
}

void WeatherUi::on_searchButton_clicked()
{
    // 切换搜索面板的可见状态
    ui->searchButton->hide();
    const bool show = !ui->searchPanel->isVisible();
    ui->searchPanel->setVisible(show);
    // 如果显示搜索面板，设置城市输入框为焦点
    if (show && ui->cityLineEdit) {
        ui->cityLineEdit->setFocus();
    }
}


void WeatherUi::on_confirmSearchButton_clicked()
{
    // 检查城市输入框是否存在
    if (!ui->cityLineEdit) {
        return;
    }
    // 获取并修剪输入的城市名
    const QString city = ui->cityLineEdit->text().trimmed();
    // 如果城市名为空，直接返回
    if (!city.isEmpty()) {
        // 更新城市天气标签
        if (ui->cityWeatherLabel) {
            ui->cityWeatherLabel->setText(city);
        }
        // 调用天气服务获取该城市的天气
        weatherService->getweatherForCity(city);
    }

    // 隐藏搜索面板
    ui->searchPanel->setVisible(false);
    ui->searchButton->show();
}

