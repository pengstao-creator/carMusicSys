#ifndef WALLPAERWIDGET_H
#define WALLPAERWIDGET_H

#include <QObject>
#include <QString>
#include <QSize>
#include <memory>

class VideoWallpaperPlayer;
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
    void stop();
    void play();
    void pause();
    const QString &getFile() const;

protected:
    void resizeEvent() ;
private:
    void setPlayer();
    void setBackground(const QString &filePath,VideoWallpaperPlayer * player);


    zAxisControl *zAxis_Ctrl;
    std::unique_ptr<VideoWallpaperPlayer> m_player_1;
    std::unique_ptr<VideoWallpaperPlayer> m_player_2;
    bool is_player_1;
    PlayerType ptype;
    QString m_currentFile;              // 当前文件路径
    QSize m_lastSceneSize;
};
#endif // WALLPAERWIDGET_H
