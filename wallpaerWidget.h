#ifndef WALLPAERWIDGET_H
#define WALLPAERWIDGET_H

#include <QObject>
#include <QString>
#include <memory>

//前置声明,防止头文件循环包含和减少编译依赖，加快编译速度
class Player;
enum class PlayerType;
class zAxisControl;
class wallpaerWidget : public QObject
{
    Q_OBJECT
public:
    explicit wallpaerWidget(zAxisControl *zAxisCtrl, QObject *parent = nullptr);
    ~wallpaerWidget();
    // 设置背景文件（支持 .png .jpg .gif .mp4）
    void setPath(const QString &filePath);
    void setPathFirst(const QString &filePath1,const QString &filePath2);
    const QString &getFile() const;
protected:
    void resizeEvent() ;
private:
    void setPlayer();
    void setBackground(const QString &filePath,Player * player);


    zAxisControl *zAxis_Ctrl;
    std::unique_ptr<Player> m_player_1;            // 播放器
    std::unique_ptr<Player> m_player_2;
    bool is_player_1;
    PlayerType ptype;
    QString m_currentFile;              // 当前文件路径
};
#endif // WALLPAERWIDGET_H
