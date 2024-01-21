#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

typedef struct r {
    QString text;
    int stars;
} review;

class DBManager : public QObject
{
    Q_OBJECT

public:
    DBManager(const QString &path, QObject * parent = nullptr);
    ~DBManager();

    void fastForward(int);
    review nextReview();

    bool errors();
    QString lastError();

    int numberOfReviews();

private:
    void updateError(QSqlQuery*);

    QSqlDatabase m_db;
    QStringList reviews;
    QSqlQuery * reviewQ;
    int reviewTextPtr;
    int reviewStarPtr;
    bool m_errors;
    QSqlError m_lastError;

    const QString getReviews = "SELECT RevText, Rating FROM REVIEW";
};

#endif // DBMANAGER_H
