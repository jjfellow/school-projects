#include "dbmanager.h"
#include <QDebug>

DBManager::DBManager(const QString &path, QObject *parent):
    QObject{parent}
    , m_errors{false}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if(!m_db.open())
    {
        qDebug() << "Error: connection with database failed!";
        m_errors = true;
        m_lastError = m_db.lastError();
    }
    else {
        qDebug() << "Database connection okay";
        reviewQ = new QSqlQuery(getReviews, m_db);
        updateError(reviewQ);
    }
}

DBManager::~DBManager()
{
    reviewQ->finish();
    m_db.close();
}

void DBManager::updateError(QSqlQuery *q) {
    m_lastError = q->lastError();
    if(m_lastError.type() != QSqlError::ErrorType::NoError) {
        m_errors = true;
    }
}

void DBManager::fastForward(int ffNum)
{
    for(int i=0; i<ffNum-1; i++) {
        if(!reviewQ->next()) {
            break;
        }
    }

    updateError(reviewQ);
}

review DBManager::nextReview()
{
    if(reviewQ->next())
    {
        updateError(reviewQ);
        review r;
        r.text = reviewQ->value(0).toString();
        r.stars = reviewQ->value(1).toInt();
        return r;
    }

    updateError(reviewQ);
    return review{"", -1};
}

bool DBManager::errors() { return m_errors; }
QString DBManager::lastError() { return m_lastError.text(); }
int DBManager::numberOfReviews() {return 39000000;}
