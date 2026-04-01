#ifndef MUSICUI_H
#define MUSICUI_H

#include "softwareuibase.h"

#include <QStringList>
#include <QVector>
#include <QSet>
class QAudioOutput;  // 前向声明Qt音频输出类
class QMediaPlayer;  // 前向声明Qt媒体播放器类
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

    struct MusicInfo
    {
        double Duration;
        QString Title;
        QString Artist;
        QString filepath;
    };

    void initSet();
    void setBackground(const QString& iconpath);



    Ui::musicUi *ui;  // UI对象指针
    QMediaPlayer *player;  // 媒体播放器指针
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *audioOutput;  // Qt6音频输出指针
#endif    
    QList<MusicInfo> musicInfos;//所有添加音乐的信息
    QSet<QString> musicPaths;//用于根据文件路径进行去重
    int currentIndex;  // 当前播放索引
    QVector<std::pair<PlayMode,QPixmap>> playMode;  // 播放模式
    int modeIndex;
    bool userSeeking;  // 用户是否正在拖动进度条
};

#endif // MUSICUI_H
