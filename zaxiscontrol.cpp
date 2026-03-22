#include "zaxiscontrol.h"
#include <QHash>
#include "Data.h"
#include "Overlay.h"
zAxisControl::zAxisControl(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
{
    setBaseQWidget();
}

zAxisControl::~zAxisControl()
{
    // 清理资源
    qDeleteAll(m_overlay.values());
    m_overlay.clear();
}

const QHash<QString,Overlay*>& zAxisControl::getOvrlay() const
{
    return m_overlay;
}

QGraphicsScene *zAxisControl::getScene() const
{
    return m_scene;
}

void zAxisControl::addOverlay(const QString &name, QWidget *widget)
{
    addOvrlay({name,new Overlay});
    m_overlay[name]->addWidget(widget);
}

void zAxisControl::addOvrlay(const std::pair<QString, Overlay *> &overlay)
{
    auto _overlay = overlay.second;
    double LAYER_PALYER = Layer::LAYER_PLAYER_3 + m_overlay.size();
    // 3. 创建透明覆盖层（一个空的 QGraphicsWidget）
    _overlay->setZValue(LAYER_PALYER);                     // 确保覆盖层在最上面
    _overlay->setGeometry(QRectF(0, 0,width(), height()));
    m_scene->addItem(_overlay);
    m_overlay.insert(overlay.first, overlay.second);
}

const QRectF& zAxisControl::getQRect() const
{
   return m_scene->sceneRect();
}

void zAxisControl::setBaseQWidget()
{
    //1.创建场景
    setScene(m_scene);
    // 2. 设置视图属性
    setFrameStyle(QFrame::NoFrame);               // 无边框
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);        // 抗锯齿
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // 避免残影
    setRenderHint(QPainter::SmoothPixmapTransform); // 平滑缩放
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
