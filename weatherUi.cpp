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
#include <QTimer>
#include <QFileInfo>
#include <QImageReader>
#include <QPalette>
#include <QBrush>
#include <QDebug>
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
constexpr int kDayCardCount = 7;

void setSvgLabelIcon(QLabel *label, const QString &iconPath, int iconSize)
{
    if (!label) return;
    label->setFixedSize(iconSize, iconSize);

    QPixmap pixmap(iconSize, iconSize);
    pixmap.fill(Qt::transparent);

    QSvgRenderer renderer(iconPath);
    if (renderer.isValid()) {
        QPainter painter(&pixmap);
        renderer.render(&painter);
        label->setPixmap(pixmap.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
}

WeatherUi::WeatherUi(QWidget *parent) : QWidget(parent)
    , weatherService(new weatherAPI(this))
    , currentCityName(QString::fromUtf8(carMusicSysconfig::DEFAULT_CITY))
    , errorTipLabel(nullptr)
    , backgroundLabel(nullptr)
    , errorTipTimer(new QTimer(this))
    , rebuildPending(false)
    , ui(new Ui::WeatherUi)
{
    qDebug() << "WeatherUi::ctor" << this << "parent" << parent;
    setupUI();
    // 天气页背景加载入口：
    // 优先使用配置项 WEATHER_BACKGROUND（当前为 7.png），
    // 若运行环境无法解码 PNG，会在 setBackground 内自动回退到 5.jpg。
    const QString backgroundPath = QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH)
                                 + QString::fromUtf8(carMusicSysconfig::WEATHER_BACKGROUND);
    setBackground(backgroundPath);

    connect(weatherService, &weatherAPI::weatherDataReady, this, &WeatherUi::updateWeather);
    connect(weatherService, &weatherAPI::requestFailed, this, [this](const QString &strerr) {
        if (ui->cityLineEdit) {
            ui->cityLineEdit->clear();
        }
        pendingCityName.clear();
        if (errorTipLabel) {
            errorTipLabel->setText(strerr.isEmpty() ? QStringLiteral("请求失败") : strerr);
            errorTipLabel->adjustSize();
            errorTipLabel->move((width() - errorTipLabel->width()) / 2, 8);
            errorTipLabel->show();
        }
        if (errorTipTimer) {
            errorTipTimer->stop();
            errorTipTimer->start(2000);
        }
    });
    connect(errorTipTimer, &QTimer::timeout, this, [this]() {
        if (errorTipLabel) {
            errorTipLabel->hide();
        }
    });
    errorTipTimer->setSingleShot(true);
    QTimer::singleShot(0, this, [this]() {
        weatherService->getweatherForCity(currentCityName);
    });
}

WeatherUi::~WeatherUi()
{
    qDebug() << "WeatherUi::dtor" << this;
    delete ui;
}

void WeatherUi::setupUI()
{
    // 初始化UI布局
    ui->setupUi(this);
    resize(kBaseWindowWidth, kBaseWindowHeight);
    // 设置窗口对象名，用于样式表
    setObjectName("weatherMainWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    // 增加一层底图 QLabel（放在最底层）：
    // 在 Qt5 + QGraphicsProxyWidget 场景中，纯样式背景可能被视口/子控件覆盖，
    // 因此使用真实像素层作为第一重保障。
    backgroundLabel = new QLabel(this);
    backgroundLabel->setGeometry(rect());
    backgroundLabel->setScaledContents(false);
    backgroundLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    backgroundLabel->lower();

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
        scrollArea->setFrameShape(QFrame::NoFrame);
        // 将滚动区域与 viewport 设为透明，避免遮住底图层。
        // 否则天气卡片之外的区域可能被默认底色填充。
        scrollArea->setAutoFillBackground(false);
        scrollArea->setAttribute(Qt::WA_TranslucentBackground, true);
        if (scrollArea->viewport()) {
            scrollArea->viewport()->setAutoFillBackground(false);
            scrollArea->viewport()->setAttribute(Qt::WA_TranslucentBackground, true);
            scrollArea->viewport()->setStyleSheet("background: transparent;");
        }
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
        ui->cityWeatherLabel->setText(currentCityName + QStringLiteral("天气"));
    }

    errorTipLabel = new QLabel(this);
    errorTipLabel->setStyleSheet(QStringLiteral("color: white; background: rgba(200, 30, 30, 180); border-radius: 8px; padding: 6px 12px;"));
    errorTipLabel->setAlignment(Qt::AlignCenter);
    errorTipLabel->hide();

    dayCards.clear();
    for (int i = 1; i <= kDayCardCount; ++i) {
        QWidget *card = findChild<QWidget*>(QString("weatherDayCard_%1").arg(i));
        if (card) {
            dayCards.append(card);
        }
    }
}

void WeatherUi::updateWeather(const QVector<QVector<QString>> &weekForecast)
{
    latestForecast = weekForecast;
    if (!pendingCityName.isEmpty()) {
        currentCityName = pendingCityName;
        pendingCityName.clear();
        if (ui->cityWeatherLabel) {
            ui->cityWeatherLabel->setText(currentCityName + QStringLiteral("天气"));
        }
    }
    scheduleRebuildCards();
}

void WeatherUi::scheduleRebuildCards()
{
    if (rebuildPending) {
        return;
    }
    rebuildPending = true;
    QTimer::singleShot(0, this, [this]() {
        rebuildPending = false;
        rebuildWeatherCards();
    });
}

void WeatherUi::rebuildWeatherCards()
{
    for (const QVector<QString> &dayData : latestForecast) {
        Q_UNUSED(dayData);
    }

    const qreal scale = qMin(static_cast<qreal>(width()) / kBaseWindowWidth,
                             static_cast<qreal>(height()) / kBaseWindowHeight);
    const qreal effectiveScale = qMax<qreal>(0.6, scale);
    const int scaledCardWidth = qMax(120, qRound(kCardWidth * effectiveScale));
    const int scaledCardSpacing = qMax(6, qRound(kCardSpacing * effectiveScale));
    const int cardCount = dayCards.size();
    const int contentWidth = (cardCount * scaledCardWidth) + (qMax(0, cardCount - 1) * scaledCardSpacing);
    cardsLayout->setSpacing(scaledCardSpacing);
    const int cardMargin = qMax(8, qRound(kCardMargin * effectiveScale));
    const int cardInnerSpacing = qMax(6, qRound(kCardInnerSpacing * effectiveScale));
    const int iconSize = qMax(18, qRound(kIconSize * effectiveScale));
    const int dateFontSize = qMax(10, qRound(kDateFontSize * effectiveScale));
    const int textFontSize = qMax(9, qRound(kTextFontSize * effectiveScale));

    for (int i = 0; i < dayCards.size(); ++i) {
        QWidget *card = dayCards[i];
        const int dayIndex = i + 1;
        card->setFixedWidth(scaledCardWidth);
        card->setVisible(i < latestForecast.size());

        QVBoxLayout *cardLayout = qobject_cast<QVBoxLayout*>(card->layout());
        if (cardLayout) {
            cardLayout->setContentsMargins(cardMargin, cardMargin, cardMargin, cardMargin);
            cardLayout->setSpacing(cardInnerSpacing);
        }

        QLabel *dateLabel = findChild<QLabel*>(QString("dateLabel_%1").arg(dayIndex));
        QLabel *amIconLabel = findChild<QLabel*>(QString("amIcon_%1").arg(dayIndex));
        QLabel *amTextLabel = findChild<QLabel*>(QString("amText_%1").arg(dayIndex));
        QLabel *pmIconLabel = findChild<QLabel*>(QString("pmIcon_%1").arg(dayIndex));
        QLabel *pmTextLabel = findChild<QLabel*>(QString("pmText_%1").arg(dayIndex));

        if (dateLabel) dateLabel->setStyleSheet(QString("color: white; font-size: %1px; font-weight: bold;").arg(dateFontSize));
        if (amTextLabel) amTextLabel->setStyleSheet(QString("color: white; font-size: %1px;").arg(textFontSize));
        if (pmTextLabel) pmTextLabel->setStyleSheet(QString("color: white; font-size: %1px;").arg(textFontSize));

        if (i >= latestForecast.size()) {
            if (dateLabel) dateLabel->clear();
            if (amTextLabel) amTextLabel->clear();
            if (pmTextLabel) pmTextLabel->clear();
            if (amIconLabel) amIconLabel->clear();
            if (pmIconLabel) pmIconLabel->clear();
            continue;
        }

        const QVector<QString> &dayData = latestForecast[i];
        if (dayData.size() < 7) continue;

        if (dateLabel) dateLabel->setText(dayData[0]);
        if (amTextLabel) amTextLabel->setText(dayData[2] + " " + dayData[3] + "°C");
        if (pmTextLabel) pmTextLabel->setText(dayData[5] + " " + dayData[6] + "°C");
        setSvgLabelIcon(amIconLabel, dayData[1], iconSize);
        setSvgLabelIcon(pmIconLabel, dayData[4], iconSize);
    }
    scrollContent->setMinimumWidth(contentWidth);
    scrollContent->adjustSize();
}

void WeatherUi::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (backgroundLabel) {
        backgroundLabel->setGeometry(rect());
    }
    // 尺寸变化时同步刷新两套背景：
    // 1) backgroundLabel 实图层；
    // 2) QPalette::Window 画刷层（Qt5 下对某些风格更稳定）。
    if (!backgroundPixmap.isNull()) {
        const QPixmap scaled = backgroundPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        if (backgroundLabel) {
            backgroundLabel->setPixmap(scaled);
        }
        QPalette pal = palette();
        pal.setBrush(QPalette::Window, QBrush(scaled));
        setPalette(pal);
        setAutoFillBackground(true);
    }
    if (errorTipLabel && errorTipLabel->isVisible()) {
        errorTipLabel->move((width() - errorTipLabel->width()) / 2, 8);
    }
    if (!latestForecast.isEmpty()) {
        scheduleRebuildCards();
    }
}

void WeatherUi::setBackground(const QString &backgroundPath)
{
    // 背景加载策略：
    // - 首选配置背景（通常是 7.png）；
    // - 若 QPixmap 为空（例如 Qt5 缺少 qpng 插件），自动回退到 5.jpg；
    // - 同时将最终可用图设置到 label + palette + stylesheet，三重兜底。
    backgroundImagePath = backgroundPath;
    backgroundPixmap = QPixmap(backgroundImagePath);
    if (backgroundPixmap.isNull()) {
        // 回退背景用于保证“至少可见”：
        // 即便 PNG 解码失败，天气页依然有背景，不影响主流程体验。
        const QString fallbackPath = QString::fromUtf8(carMusicSysconfig::WEATHER_APP_PATH) + "7.jpg";
        backgroundPixmap = QPixmap(fallbackPath);
        if (!backgroundPixmap.isNull()) {
            backgroundImagePath = fallbackPath;
        }
    }
    if (backgroundLabel) {
        if (!backgroundPixmap.isNull()) {
            const QPixmap scaled = backgroundPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            backgroundLabel->setPixmap(scaled);
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, QBrush(scaled));
            setPalette(pal);
            setAutoFillBackground(true);
        } else {
            backgroundLabel->setPixmap(QPixmap());
            setAutoFillBackground(false);
        }
    }
    // 样式层仍保留，主要用于统一圆角卡片之外区域的绘制行为。
    const QString style = QString(
        "#weatherMainWidget {"
        "background-image: url(%1);"
        "background-position: center;"
        "background-repeat: no-repeat;"
        "}"
        "QScrollArea { background: transparent; border: none; }"
        "QScrollArea > QWidget > QWidget { background: transparent; }")
        .arg(backgroundImagePath);
    setStyleSheet(style);
    // 关键诊断日志：
    // exists=true 但 pixmapNull=true 通常意味着运行时图像解码插件缺失。
    qDebug() << "WeatherUi::setBackground" << backgroundImagePath << "exists" << QFileInfo(backgroundImagePath).exists() << "pixmapNull" << backgroundPixmap.isNull() << "size" << backgroundPixmap.size();
    if (backgroundPixmap.isNull()) {
        qDebug() << "WeatherUi::imageFormats" << QImageReader::supportedImageFormats();
    }
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
    qDebug() << "WeatherUi::on_exitButton_clicked" << this;
    close();
    emit exit();
}

void WeatherUi::on_searchButton_clicked()
{
    qDebug() << "WeatherUi::on_searchButton_clicked" << this;
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
    qDebug() << "WeatherUi::on_confirmSearchButton_clicked" << this;
    // 检查城市输入框是否存在
    if (!ui->cityLineEdit) {
        return;
    }
    // 获取并修剪输入的城市名
    const QString city = ui->cityLineEdit->text().trimmed();
    if (!city.isEmpty()) {
        pendingCityName = city;
        // 调用天气服务获取该城市的天气
        weatherService->getweatherForCity(city);
    }

    // 隐藏搜索面板
    ui->searchPanel->setVisible(false);
    ui->searchButton->show();
}

