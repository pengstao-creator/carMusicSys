#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QGraphicsView>
#include <QString>
#include <memory>

//前置声明,防止头文件循环包含和减少编译依赖，加快编译速度
class QGraphicsScene;
class QGraphicsWidget;
class Overlay;
class Player;
enum class PlayerType;
class BackgroundWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit BackgroundWidget(QWidget *parent = nullptr);
    ~BackgroundWidget();
    // 设置背景文件（支持 .png .jpg .gif .mp4）
    void setPath(const QString &filePath);
    void setPathFirst(const QString &filePath1,const QString &filePath2);
    const QString &getFile() const;
    // 获取覆盖层（一个 QGraphicsWidget，可在其上放置其他图形项或代理控件）
    static const QHash<QString,Overlay*>& getOvrlay() ;
    QGraphicsScene * getScene()const;


    void addOvrlay(const std::pair<QString,Overlay*>& overlay);
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setBaseQWidget();
    void setPlayer();
    void setBackground(const QString &filePath,Player * player,bool isplayer);

    std::unique_ptr<QGraphicsScene> m_scene;            // 场景
    std::unique_ptr<Player> m_player_1;            // 播放器
    std::unique_ptr<Player> m_player_2;
    bool is_player_1;
    PlayerType ptype;
    QString m_currentFile;              // 当前文件路径
    QHash<QString,Overlay*> m_overlay;         // 覆盖层（透明容器）
    static BackgroundWidget* _this;
};
#endif // BACKGROUNDWIDGET_H
