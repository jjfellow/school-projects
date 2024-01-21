#ifndef NBCLASSIFIER_H
#define NBCLASSIFIER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QProcess>
#include <QMap>
#include <QMutex>

#include "classifyprocess.h"
#include "dbmanager.h"

class NBClassifier : public QObject
{
    Q_OBJECT

    typedef struct wi {
        int num5reviews;
        int num1reviews;
        double postProb5 = 0.0;
    }wordInfo;

public:
    explicit NBClassifier(QObject *parent = nullptr);
    ~NBClassifier();

    void setDB(DBManager *);
    void saveProgress();

private:
    QJsonObject ObjectFromString(const QString& in);
    void execF();
    void execDB();
    void savePrivate();

    DBManager *db;

    QFile file;
    QMap<QString, wordInfo> infoMap;
    QHash<QString, int> word2ExitCode;
    qint64 five;
    quint64 one;
    quint64 processesStarted;
    quint64 processesFinished;
    int processesRunning;
    bool run;
    QMutex mutex;
    quint64 global_count = 0;

    const QString FILE_NAME = "../../review-Texas.json";
    const int MAX_PARALLEL_PROCESS = 10;
    const QString ORG_NAME = "MLClass";
    const QString APP_NAME = "NaiveBayes";
    const QString SAVE_CSV = "naiveBayes.csv";

public slots:
    void exec();
    void endProcess(int exitCode, reviewInfo ri);

signals:
    void execDone();
};

#endif // NBCLASSIFIER_H
