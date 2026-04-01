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
class softwareUiBase : public QWidget
{
    Q_OBJECT
public:
    explicit softwareUiBase(QWidget *parent = nullptr);
    qreal uiScaleFactor() const;
    void setDesignSize(const QSize &size);
    QSize designSize() const;
    static const QString& getSoftname();
    static const QPixmap& getSofticon();
    static softwareUiBase * getSingleton();
signals:
    void exit();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void onUiScaleChanged(qreal scaleFactor);
    QLabel * backgroundLabel;
private:
    QSize m_designSize;
    qreal m_uiScaleFactor;

};

#endif // SOFTWAREUIBASE_H
