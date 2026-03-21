#ifndef OVERLAY_H
#define OVERLAY_H

#include <QGraphicsProxyWidget>

namespace Ui {
class Overlay;
}

class Overlay : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    explicit Overlay(QGraphicsItem *parent = nullptr);
    ~Overlay();

private:
    Ui::Overlay *ui;
};

#endif // OVERLAY_H