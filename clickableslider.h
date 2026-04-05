#ifndef CLICKABLESLIDER_H
#define CLICKABLESLIDER_H

#include <QSlider>

class QMouseEvent;

class ClickableSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ClickableSlider(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int valueFromPosition(QMouseEvent *event) const;
};

#endif // CLICKABLESLIDER_H
