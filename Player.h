#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QSize>
#include <memory>
class QString;
class QGraphicsPixmapItem;
class QGraphicsVideoItem;
class QMovie;
class QMediaPlayer;

// 条件编译以支持Qt5
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QAudioOutput;
#endif

enum class PlayerType {
    NONPLAYER,
    PIXMAP,
    MOVIE,
    VIDEO
};

class VideoWallpaperPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoWallpaperPlayer(QObject *parent = nullptr);
    ~VideoWallpaperPlayer();
    
    void setupPixmap(const QString &path, const QSize &targetSize);   // 静态图片
    void refreshPixmap(const QSize &targetSize);
    void setupMovie(const QString &path);    // GIF
    void setupVideo(const QString &path);    // MP4
    void setWallpaperPlayer(const qreal z);
    void pause();
    void stop();
    void play();
    bool hidePlayer(PlayerType); 
    bool showPlayer(PlayerType);
    
    // 访问器方法
    QGraphicsPixmapItem* getPixmapItem() const { return m_pixmapItem; }
    QGraphicsVideoItem* getVideoItem() const { return m_videoItem; }
    QMediaPlayer* getMediaPlayer() const { return m_mediaPlayer.get(); }
    PlayerType getCurrentPlayerType() const { return ptype; }
    
    // 设置视频项大小
    void setVideoSize(const QSize &size);

private:
    void setPixmapPlayer(const qreal z);
    void setVideoPlayer(const qreal z);
    void setMoviePlayer(const qreal z);
    void setAudioPlayer();
    void applyCachedPixmap(const QString &path, const QSize &targetSize);
    QGraphicsPixmapItem *m_pixmapItem;  // 由 QGraphicsScene 托管
    QGraphicsVideoItem *m_videoItem;    // 由 QGraphicsScene 托管
    std::unique_ptr<QMovie> m_movie;                    // GIF播放器
    QGraphicsPixmapItem *m_MovieItem;    // 由 QGraphicsScene 托管
    std::unique_ptr<QMediaPlayer> m_mediaPlayer;        // 视频播放器
    // 条件编译以支持Qt5
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    std::unique_ptr<QAudioOutput> m_audioOutput;        // 音频输出
    #endif

    PlayerType ptype;                                   // 当前播放器类型
    QString m_currentPixmapPath;
    QString m_currentVideoPath;
    QSize m_lastVideoSize;
    bool m_videoSignalsConnected;
    bool m_movieSignalsConnected;
};

#endif // PLAYER_H
