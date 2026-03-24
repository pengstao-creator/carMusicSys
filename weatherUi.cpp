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
#include <QScrollArea>
#include <QSvgRenderer>
#include <QtGlobal>
#include <QVBoxLayout>

namespace {
constexpr int kCardWidth = 160;
constexpr int kCardSpacing = 15;
constexpr int kCardMargin = 16;

QLabel *createSvgLabel(const QString &iconPath, QWidget *parent)
{
    QLabel *label = new QLabel(parent);
    label->setFixedSize(32, 32);

    QPixmap pixmap(32, 32);
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
    // 设置窗口固定大小为800×400
    setFixedSize(800, 400);
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
    // 创建卡片容器
    QWidget *card = new QWidget(scrollContent);
    card->setObjectName("weatherDayCard");
    card->setFixedWidth(kCardWidth);
    card->setStyleSheet(
        "QWidget#weatherDayCard { "
        "    background: rgba(255, 255, 255, 0.30); "
        "    border-radius: 14px; "
        "}"
    );

    // 创建卡片内部布局
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(kCardMargin, kCardMargin, kCardMargin, kCardMargin);
    cardLayout->setSpacing(12);

    // 日期标签
    QLabel *dateLabel = new QLabel(date, card);
    dateLabel->setObjectName("weatherDateLabel");
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setStyleSheet("QLabel#weatherDateLabel { color: white; font-size: 14px; font-weight: bold; }");
    cardLayout->addWidget(dateLabel);

    // 早上天气
    QWidget *amWidget = new QWidget(card);
    amWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *amLayout = new QHBoxLayout(amWidget);
    amLayout->setContentsMargins(0, 0, 0, 0);
    amLayout->setSpacing(8);

    QLabel *amSvg = createSvgLabel(amIcon, amWidget);
    QLabel *amLabel = new QLabel(amText, amWidget);
    amLabel->setStyleSheet("color: white; font-size: 12px;");

    amLayout->addWidget(amSvg);
    amLayout->addWidget(amLabel);
    cardLayout->addWidget(amWidget);

    // 晚上天气
    QWidget *pmWidget = new QWidget(card);
    pmWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *pmLayout = new QHBoxLayout(pmWidget);
    pmLayout->setContentsMargins(0, 0, 0, 0);
    pmLayout->setSpacing(8);

    QLabel *pmSvg = createSvgLabel(pmIcon, pmWidget);
    QLabel *pmLabel = new QLabel(pmText, pmWidget);
    pmLabel->setStyleSheet("color: white; font-size: 12px;");

    pmLayout->addWidget(pmSvg);
    pmLayout->addWidget(pmLabel);
    cardLayout->addWidget(pmWidget);

    cardsLayout->addWidget(card);
    dayCards.append(card);
}

void WeatherUi::updateWeather(const QVector<QVector<QString>> &weekForecast)
{
    // 清空现有卡片
    for (QWidget *card : dayCards) {
        cardsLayout->removeWidget(card);
        delete card;
    }
    dayCards.clear();

    // 创建新卡片（不足 7 天时补占位，保持完整界面）
    for (const QVector<QString> &dayData : weekForecast) {
        if (dayData.size() >= 7) {
            QString date = dayData[0];
            QString amIcon = dayData[1];
            QString amText = dayData[2] + " " + dayData[3] + "°C";
            QString pmIcon = dayData[4];
            QString pmText = dayData[5] + " " + dayData[6] + "°C";
            
            createDayCard(date, amIcon, amText, pmIcon, pmText);
        }
    }

    // 依据卡片数设置内容最小宽度，保证横向滚动行为稳定
    const int cardCount = dayCards.size();
    const int contentWidth = (cardCount * kCardWidth) + (qMax(0, cardCount - 1) * kCardSpacing);
    scrollContent->setMinimumWidth(contentWidth);
    scrollContent->adjustSize();
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
    if (city.isEmpty()) {
        return;
    }
    // 更新城市天气标签
    if (ui->cityWeatherLabel) {
        ui->cityWeatherLabel->setText(city);
    }
    // 调用天气服务获取该城市的天气
    weatherService->getweatherForCity(city);
    // 隐藏搜索面板
    if (ui->searchPanel) {
        ui->searchPanel->setVisible(false);
    }
}

