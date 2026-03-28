#include "zaxiscontrol.h"
#include <QHash>
#include "Data.h"
#include "Overlay.h"
#include <QDebug>

// 条件编译以支持Qt5
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#endif
zAxisControl::zAxisControl(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
{
    setBaseQWidget();
}

zAxisControl::~zAxisControl()
{
    qDebug() << "zAxisControl::dtor overlays" << m_overlay.keys();
    // 清理资源
    qDeleteAll(m_overlay.values());
}

const QHash<QString,Overlay*>& zAxisControl::getOvrlay() const
{
    return m_overlay;
}

QGraphicsScene *zAxisControl::getScene() const
{
    return m_scene;
}

Overlay *zAxisControl::getOverlay(const QString &name) const
{
    return m_overlay.value(name, nullptr);
}

void zAxisControl::addOvrlay(const QString &name, QWidget *widget,bool is_transparent)
{
    qDebug() << "zAxisControl::addOverlay" << name << "widget" << widget << "transparent" << is_transparent;
    if (m_overlay.contains(name)) {
        return;
    }
    addOvrlay({name,new Overlay});
    m_overlay[name]->addWidget(widget,is_transparent);
}

void zAxisControl::addOvrlay(const std::pair<QString, Overlay *> &overlay)
{
    qDebug() << "zAxisControl::addOvrlay" << overlay.first << overlay.second;
    auto _overlay = overlay.second;
    double LAYER_PALYER = Layer::LAYER_PLAYER_3 + m_overlay.size();
    _overlay->setZValue(LAYER_PALYER);                     // 确保覆盖层在最上面
    _overlay->setGeometry(QRectF(0, 0,width(), height()));
    m_scene->addItem(_overlay);
    m_overlay.insert(overlay.first, overlay.second);
}

void zAxisControl::erase(const QString &name)
{
    qDebug() << "zAxisControl::erase" << name;
    auto it = m_overlay.find(name);
    if (it == m_overlay.end()) {
        return;
    }
    Overlay *overlay = it.value();
    m_overlay.erase(it);
    if (!overlay) {
        return;
    }
    if (m_scene) {
        m_scene->removeItem(overlay);
    }
    delete overlay;
}

QRectF zAxisControl::getQRect() const
{
   return m_scene->sceneRect();
}

void zAxisControl::setBaseQWidget()
{
    //1.创建场景
    setScene(m_scene);
    
    // 条件编译以支持Qt5
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setViewport(new QOpenGLWidget(this));
    #endif
    
    // 2. 设置视图属性
    setFrameStyle(QFrame::NoFrame);               // 无边框
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing, false);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 条件编译以支持Qt5
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    #endif
    
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setAlignment(Qt::AlignCenter);
    // 3. 设置背景色（默认黑色）
    setBackgroundBrush(Qt::black);

}


void zAxisControl::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (m_scene) {
        // 调整场景大小
        m_scene->setSceneRect(0, 0, width(), height());
        //调整覆盖层大小
        if (!m_overlay.isEmpty()) {
            for(auto overlay : m_overlay)
            {
                overlay->setGeometry(QRectF(0, 0, width(), height()));
            }
        }
    }
    emit resized();
}
