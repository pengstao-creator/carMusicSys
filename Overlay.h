#ifndef OVERLAY_H
#define OVERLAY_H

#include <QGraphicsProxyWidget>
class Overlay : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    explicit Overlay(QGraphicsItem *parent = nullptr);
    ~Overlay();
    void addWidget(QWidget * overlyD,bool is_transparent = false);
private:
};

#endif // OVERLAY_H
