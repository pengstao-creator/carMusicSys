#ifndef SOFTWAREUIBASE_H
#define SOFTWAREUIBASE_H

#include <QSize>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class QPixmap;
class QResizeEvent;
class QString;
class QLabel;
class zAxisControl;
class softwareUiBase : public QWidget
{
    Q_OBJECT
public:
    explicit softwareUiBase(QWidget *parent = nullptr);
    qreal uiScaleFactor() const;
    void setDesignSize(const QSize &size);
    void setzAxisControl(zAxisControl * zAxisCtrl);
    QSize designSize() const;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
signals:
    void exit();   
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void onUiScaleChanged(qreal scaleFactor);
    QLabel * backgroundLabel;
    QPixmap backgroundPixmap;
    zAxisControl * _zAxisCtrl;
private:
    QSize m_designSize;
    qreal m_uiScaleFactor;

};

#endif // SOFTWAREUIBASE_H
