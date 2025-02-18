#include "authorrepository.h"
#include <QSqlQuery>
#include <QVariant>
#include <src/factories/authorfactory.h>
#include <QSqlError>
#include <QDebug>

Author AuthorRepository::get(int id)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE id = ?");
    q.addBindValue(QVariant::fromValue(id));
    q.exec();

    bool ret = q.next();
    if (ret) {
        return AuthorFactory::fromSqlRecord(q.record());
    } else {
        qWarning() << "Couldn't fetch";
    }

    return {};
}

void AuthorRepository::put(Author &entity)
{
    QSqlQuery q;
    q.prepare("INSERT INTO author(authorId, name, url, avatar, subscribed) VALUES (?,?,?,?,?)");
    q.addBindValue(QVariant::fromValue(entity.authorId));
    q.addBindValue(QVariant::fromValue(entity.name));
    q.addBindValue(QVariant::fromValue(entity.url));
    q.addBindValue(QVariant::fromValue(entity.bestAvatar.url));
    q.addBindValue(QVariant::fromValue(entity.subscribed));
    q.exec();

    QVariant v = q.lastInsertId();
    Q_ASSERT(v.isValid());

    entity.id = v.toInt();
}

void AuthorRepository::update(Author entity)
{
    if (entity.id == -1) return;

    QSqlQuery q;
    q.prepare("UPDATE author set authorId = ?, name = ?, url = ?, avatar = ?, subscribed = ? WHERE id = ?");

    q.addBindValue(QVariant::fromValue(entity.authorId));
    q.addBindValue(QVariant::fromValue(entity.name));
    q.addBindValue(QVariant::fromValue(entity.url));
    q.addBindValue(QVariant::fromValue(entity.bestAvatar.url));
    q.addBindValue(QVariant::fromValue(entity.subscribed));
    q.addBindValue(QVariant::fromValue(entity.id));
    q.exec();
}

void AuthorRepository::remove(Author entity)
{
    Q_ASSERT(entity.id);

    QSqlQuery q;
    q.prepare("DELETE FROM author WHERE id = ?");
    q.addBindValue(entity.id);
    q.exec();
}

Author AuthorRepository::getOneByChannelId(QString channelId)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE authorId = ?");
    q.addBindValue(QVariant::fromValue(channelId));
    q.exec();

    bool ret = q.next();
    if (ret) {
        return AuthorFactory::fromSqlRecord(q.record());
    } else {
        qWarning() << "Couldn't fetch";
    }

    return {};
}

QVector<Author> AuthorRepository::getSubscriptions()
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE subscribed = true");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "AuthorRepository::getSubscriptions", q.lastError().text().toLatin1());

    QVector<Author> result;
    while(q.next()) {
        result.append(AuthorFactory::fromSqlRecord(q.record()));
    }

    return result;
}

QVector<Author> AuthorRepository::getSubscriptionsWithUnwatchedCount()
{
    QSqlQuery q;
    q.prepare("SELECT author.*, COUNT(video.id) as unwatchedCount FROM author LEFT JOIN video ON video.author = author.id AND video.watched = false  WHERE subscribed = true GROUP BY author.id");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "AuthorRepository::getSubscriptionsWithUnwatchedCount", q.lastError().text().toLatin1());

    QVector<Author> result;
    while(q.next()) {
        result.append(AuthorFactory::fromSqlRecord(q.record()));
    }

    return result;
}

void AuthorRepository::initTable()
{
    QSqlQuery q;
    q.prepare("create table if not exists author (id INTEGER PRIMARY KEY AUTOINCREMENT, authorId TEXT, name TEXT, url TEXT, avatar TEXT, subscribed BOOLEAN)");
    q.exec();
}
