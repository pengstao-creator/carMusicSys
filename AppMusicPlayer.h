#ifndef APPMUSICPLAYER_H
#define APPMUSICPLAYER_H

#include "AppMediaPlayerBase.h"
#include <QImage>
#include <QPixmap>
namespace TagLib {
class String;
}
class AppMusicPlayer : public AppMediaPlayerBase
{
    Q_OBJECT
public:
    struct MusicInfo
    {
        QString Duration; //时长
        QString Title; //标题
        QString Artist;//艺术家
        QString AlbumTitle;
        QPixmap CoverArt;//封面
        QString filepath;//文件路径
    };
    explicit AppMusicPlayer(QObject *parent = nullptr);
    ~AppMusicPlayer() override;
    const MusicInfo& getMusicInfo() const { return musicInfo; }
signals:
    void laodOk();
private slots:
     void getMetaData();
private:
     void cleanMusicInfo();
     // 辅助函数：将 TagLib 的字符串安全地转换为 QString
     QString tagStringToQString(const TagLib::String &ts);
     QImage getCoverFromMP3(const QString &filePath);
     QImage getCoverFromMP4(const QString &filePath);
     QImage getCoverFromOGG(const QString &filePath);



//     QList<MusicInfo> musicInfos;//所有添加音乐的信息
     MusicInfo musicInfo;//当前音乐信息

};

#endif // APPMUSICPLAYER_H
