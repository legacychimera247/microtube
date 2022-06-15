#ifndef AUTHORFACTORY_H
#define AUTHORFACTORY_H

#include <QJsonObject>
#include "entities/author.h"
#include <QSqlRecord>

class AuthorFactory
{
public:
    static Author fromJson(QJsonObject json);
    static Author fromTrendingJson(QJsonObject json);
    static Author fromSqlRecord(QSqlRecord record);
    static Author fromChannelVideosJson(QJsonObject json);
    static Author fromChannelInfoJson(QJsonObject json);
    static Author fromSubscriptionsJson(QJsonObject json);
    static Author fromCommentsJson(QJsonObject json);
};

#endif // AUTHORFACTORY_H
