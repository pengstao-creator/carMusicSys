#ifndef MUSICUI_H
#define MUSICUI_H

#include "softwareuibase.h"
#include "AppMusicPlayer.h"
#include <QStringList>
#include <QVector>
#include <QHash>
class QListWidgetItem;  // 前向声明Qt列表项类

namespace Ui {
class musicUi;  // 前向声明UI类
}


class musicUi : public softwareUiBase
{
    Q_OBJECT  // Qt元对象系统宏，支持信号槽机制
public:

    explicit musicUi(QWidget *parent = nullptr);
    

    ~musicUi();
    

    static const QString &getSoftname();
    

    static const QPixmap &getSofticon();

    static softwareUiBase *getSingleton();

private slots:

    void on_switchPlaybackType_clicked();

    void on_exitButton_clicked();

    void on_prevButton_clicked();

    void on_playPauseButton_clicked();

    void on_nextButton_clicked();

    void on_playlistButton_clicked();

private:

    enum class PlayMode {
        Sequential,  // 顺序播放
        ListLoop,    // 列表循环
        SingleLoop,  // 单曲循环
        Random       // 随机播放
    };



    void initSet();
    void setBackground(const QString& iconpath);
    void setMusicUi();


    Ui::musicUi *ui;  // UI对象指针
    int currentIndex;  // 当前播放索引
    int modeIndex;
    bool userSeeking;  // 用户是否正在拖动进度条
    AppMusicPlayer * player;
    QList<QString> musicFilePaths;//保存音乐完整路径用
    QHash<QString,int> OncePaths;//用于根据文件路径进行去重，和歌名快速搜索返回索引
    QVector<std::pair<PlayMode,QPixmap>> playMode;  // 播放模式
    QPixmap dcoverArt;
};

#endif // MUSICUI_H
