#include "clickableslider.h"

#include <QMouseEvent>
#include <QStyle>
#include <QtGlobal>
ClickableSlider::ClickableSlider(QWidget *parent) : QSlider(parent) {
    // 设置样式表
    this->setStyleSheet(
        /* 1. 整个QSlider控件的背景样式 */
        "QSlider {"
        "   background-color: transparent;"       // 背景透明
        "}"

        /* 2. 滑道 (Groove) 的样式：滑块移动的轨道 */
        "QSlider::groove:horizontal {"
        "   height: 6px;"                         // 轨道高度
        "   background: #e0e0e0;"                 // 轨道背景色
        "   border-radius: 3px;"                  // 轨道圆角
        "}"

        /* 3. 已滑动区域 (Sub-page) 的样式：滑块左侧的部分 */
        "QSlider::sub-page:horizontal {"
        "   background: #00b4ff;"                 // 已滑动区域颜色
        "   border-radius: 3px;"
        "}"

        /* 4. 滑块手柄 (Handle) 的样式：用户拖动的图标（重点！）*/
        "QSlider::handle:horizontal {"
        "   background: #ffffff;"                 // 手柄背景色
        "   border: 1px solid #5c5c5c;"           // 手柄边框
        "   width: 18px;"                         // 手柄宽度
        "   height: 18px;"                        // 手柄高度
        "   margin: -6px 0;"                      // 负外边距实现垂直居中
        "   border-radius: 9px;"                  // 圆角，设为宽/高的一半即为圆形
        "}"

        /* 5. 为不同交互状态设置样式 (可选) */
        "QSlider::handle:horizontal:hover {"
        "   background: #e6e6e6;"                 // 鼠标悬停时的颜色
        "}"
        "QSlider::handle:horizontal:pressed {"
        "   background: #cccccc;"                 // 按下时的颜色
        "}"
    );
}

void ClickableSlider::mousePressEvent(QMouseEvent *event)
{
    if (event && event->button() == Qt::LeftButton) {
        setSliderDown(true);
        emit sliderPressed();
        const int value = valueFromPosition(event);
        setSliderPosition(value);
        setValue(value);
        emit sliderMoved(value);
        event->accept();
        return;
    }
    QSlider::mousePressEvent(event);
}

void ClickableSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (event && (event->buttons() & Qt::LeftButton)) {
        const int value = valueFromPosition(event);
        setSliderPosition(value);
        setValue(value);
        emit sliderMoved(value);
        event->accept();
        return;
    }
    QSlider::mouseMoveEvent(event);
}

void ClickableSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event && event->button() == Qt::LeftButton) {
        const int value = valueFromPosition(event);
        setSliderPosition(value);
        setValue(value);
        emit sliderMoved(value);
        setSliderDown(false);
        emit sliderReleased();
        event->accept();
        return;
    }
    QSlider::mouseReleaseEvent(event);
}

int ClickableSlider::valueFromPosition(QMouseEvent *event) const
{
    if (!event) {
        return value();
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPoint pos = event->position().toPoint();
#else
    const QPoint pos = event->pos();
#endif
    if (orientation() == Qt::Vertical) {
        return QStyle::sliderValueFromPosition(minimum(), maximum(), height() - pos.y(), height());
    }
    return QStyle::sliderValueFromPosition(minimum(), maximum(), pos.x(), width());
}
