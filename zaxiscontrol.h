#ifndef ZAXISCONTROL_H
#define ZAXISCONTROL_H

#include <QGraphicsView>
#include <QHash>
#include <QString>

//前置声明,防止头文件循环包含和减少编译依赖，加快编译速度
class QGraphicsScene;
class QGraphicsWidget;
class Overlay;
class zAxisControl : public QGraphicsView
{
    Q_OBJECT
public:
    explicit zAxisControl(QWidget *parent = nullptr);
    ~zAxisControl();
    const QHash<QString,Overlay*>& getOvrlay() const;
    QGraphicsScene * getScene()const;
    void addOverlay(const QString &name,QWidget *widget,bool is_transparent = false);
    void addOvrlay(const std::pair<QString,Overlay*>& overlay);
    void erase(const QString& name);
    QRectF getQRect() const;

signals:
    void wallpaperStop();
    void wallpaperStart();
    void wallpaperPause();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
        void setBaseQWidget();
    QGraphicsScene * m_scene;
    QHash<QString,Overlay*> m_overlay;

signals:
    void resized();
};

#endif // ZAXISCONTROL_H
