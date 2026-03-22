#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <memory>
class QString;
class QGraphicsPixmapItem;
class QGraphicsVideoItem;
class QMovie;
class QMediaPlayer;
class QAudioOutput;

enum class PlayerType {
    NONPLAYER,
    PIXMAP,
    MOVIE,
    VIDEO
};

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();
    
    void setupPixmap(const QString &path);   // 静态图片 
    void setupMovie(const QString &path);    // GIF
    void setupVideo(const QString &path);    // MP4
    void setPlayer(const qreal z);
    void pause();
    void stop();
    void play();
    bool switchPlayer(PlayerType); 
    bool hidePlayer(PlayerType); 
    bool showPlayer(PlayerType);
    
    // 访问器方法
    QGraphicsPixmapItem* getPixmapItem() const { return m_pixmapItem.get(); }
    QGraphicsVideoItem* getVideoItem() const { return m_videoItem.get(); }
    QMediaPlayer* getMediaPlayer() const { return m_mediaPlayer.get(); }
    PlayerType getCurrentPlayerType() const { return ptype; }
    
    // 设置视频项大小
    void setVideoSize(const QSize &size);

private:
    std::unique_ptr<QGraphicsPixmapItem> m_pixmapItem;  // 静态图片项
    std::unique_ptr<QGraphicsVideoItem> m_videoItem;    // 视频项
    std::unique_ptr<QMovie> m_movie;                    // GIF播放器
    std::unique_ptr<QMediaPlayer> m_mediaPlayer;        // 视频播放器
    std::unique_ptr<QAudioOutput> m_audioOutput;        // 音频输出

    PlayerType ptype;                                   // 当前播放器类型
};

#endif // PLAYER_H
