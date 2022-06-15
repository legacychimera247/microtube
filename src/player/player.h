#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QQuickPaintedItem>
#include <AudioResourceQt>
#include <gst/gst.h>
#include <QTimer>
#include "renderernemo.h"

class QtCamViewfinderRenderer;

class VideoPlayer : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(QUrl videoSource READ getVideoSource WRITE setVideoSource NOTIFY videoSourceChanged);
    Q_PROPERTY(QUrl audioSource READ getAudioSource WRITE setAudioSource NOTIFY audioSourceChanged);
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged);
    Q_PROPERTY(qint64 position READ getPosition WRITE setPosition NOTIFY positionChanged);
    Q_PROPERTY(State state READ getState NOTIFY stateChanged);
    Q_PROPERTY(QString subtitle READ getSubtitle WRITE setSubtitle NOTIFY subtitleChanged);
    Q_PROPERTY(QString displaySubtitle READ getDisplaySubtitle WRITE setDisplaySubtitle NOTIFY displaySubtitleChanged);
    Q_ENUMS(State);

public:
    VideoPlayer(QQuickItem *parent = 0);
    ~VideoPlayer();

    virtual void componentComplete();
    virtual void classBegin();

    void paint(QPainter *painter);

    QUrl getVideoSource() const;
    void setVideoSource(const QUrl& videoSource);
    QUrl getAudioSource() const;
    void setAudioSource(const QUrl& audioSource);
    qint64 getDuration() const;
    qint64 getPosition();
    void setPosition(qint64 position);
    QString getSubtitle() const;
    void setSubtitle(QString subtitle);
    QString getDisplaySubtitle() const;
    void setDisplaySubtitle(QString subtitle);

    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool seek(qint64 offset);
    Q_INVOKABLE bool stop();
    Q_INVOKABLE void setAudioOnlyMode(bool audioOnlyMode);

    typedef enum {
        StateStopped,
        StatePaused,
        StatePlaying,
        StateBuffering,
    } State;

    State getState() const;

signals:
    void videoSourceChanged();
    void audioSourceChanged();
    void durationChanged();
    void positionChanged();
    void error(const QString& message, int code, const QString& debug);
    void stateChanged();
    void subtitleChanged();
    void displaySubtitleChanged();

protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private slots:
    void updateRequested();
    void updateBufferingState(int percent, QString name);

private:
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    static void cbNewPad(GstElement *element, GstPad *pad, gpointer data);
    static void cbNewVideoPad(GstElement *element, GstPad *pad, gpointer data);
    static GstFlowReturn cbNewSample(GstElement *sink, gpointer *data);

    bool setState(const State& state);

    QtCamViewfinderRendererNemo *_renderer;
    AudioResourceQt::AudioResource _audioResource;
    QUrl _videoUrl;
    QUrl _audioUrl;

    GstElement *_pipeline;
    GstElement *_videoSource;
    GstElement *_audioSource;
    GstElement *_pulsesink;
    GstElement *_subParse;
    GstElement *_appSink;
    GstElement *_subSource;
    State _state;
    QTimer *_timer;
    qint64 _pos;
    bool _created;
    bool _audioOnlyMode = false;
    QString _currentSubtitle;
    QString _subtitle;
    quint64 _subtitleEnd;
    QHash<QString, int> _bufferingProgress;
};

#endif /* VIDEO_PLAYER_H */
