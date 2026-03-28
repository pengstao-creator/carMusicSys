#include "softwareuibase.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QString>
#include <QtGlobal>

softwareUiBase::softwareUiBase(QWidget *parent)
    : QWidget(parent)
    , m_designSize(800, 400)
    , m_uiScaleFactor(1.0)
{
    setAttribute(Qt::WA_NoMousePropagation, true);
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
