#ifndef COMMENTSMODEL_H
#define COMMENTSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "entities/comment.h"
#include "helpers/jsprocesshelper.h"

class CommentsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CommentsRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PhotoRole,
        TextRole,
        PublishedRole,
        RepliesCountRole,
        RepliesContinuationRole
    };

    explicit CommentsModel(QObject *parent = nullptr);
    Q_INVOKABLE void loadCommentsForVideo(QString videoId);
    Q_INVOKABLE void loadRepliesForComment(QJsonObject continuation);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void gotComments(bool canContinue, bool isContinuation);
    void gotCommentReplies(QJsonObject continuation);

protected:
    QHash<int, QByteArray> roleNames() const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    std::vector<Comment> _comments;
    bool _canContinue;
    QString _videoId;
    QJsonObject _repliesContinuation;
    JSProcessHelper _jsProcessHelper;
};

#endif // COMMENTSMODEL_H
