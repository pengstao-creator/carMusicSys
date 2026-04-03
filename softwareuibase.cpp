#include "softwareuibase.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QString>
#include <QtGlobal>
#include <QLabel>
softwareUiBase::softwareUiBase(QWidget *parent)
    : QWidget(parent)
    , m_designSize(800, 400)
    , m_uiScaleFactor(1.0)
{
    setGeometry(0,0,800,400);
    setAttribute(Qt::WA_NoMousePropagation, true);
    // 增加一层底图 QLabel（放在最底层）：
    // 在 Qt5 + QGraphicsProxyWidget 场景中，纯样式背景可能被视口/子控件覆盖，
    backgroundLabel = new QLabel(this);
    backgroundLabel->setGeometry(rect());
    backgroundLabel->setScaledContents(false);
    backgroundLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    backgroundLabel->setStyleSheet("background-color: rgb(255, 255, 255);");
    backgroundLabel->lower();
}

qreal softwareUiBase::uiScaleFactor() const
{
    return m_uiScaleFactor;
}

void softwareUiBase::setDesignSize(const QSize &size)
{
    if (size.width() <= 0 || size.height() <= 0) {
        return;
    }
    m_designSize = size;
}

QSize softwareUiBase::designSize() const
{
    return m_designSize;
}

const QString &softwareUiBase::getSoftname()
{
    static const QString softName;
    return softName;
}

const QPixmap &softwareUiBase::getSofticon()
{
    static const QPixmap softIcon;
    return softIcon;
}

softwareUiBase *softwareUiBase::getSingleton()
{
    return nullptr;
}

void softwareUiBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void softwareUiBase::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (backgroundLabel) {
        backgroundLabel->setGeometry(rect());
    }
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


    const qreal widthScale = static_cast<qreal>(width()) / m_designSize.width();
    const qreal heightScale = static_cast<qreal>(height()) / m_designSize.height();
    m_uiScaleFactor = qMax<qreal>(0.1, qMin(widthScale, heightScale));
    onUiScaleChanged(m_uiScaleFactor);
}
void softwareUiBase::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void softwareUiBase::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
}

void softwareUiBase::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

void softwareUiBase::onUiScaleChanged(qreal scaleFactor)
{
    Q_UNUSED(scaleFactor);
}
