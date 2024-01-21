#include "classifyprocess.h"
#include "QDebug"

ClassifyProcess::ClassifyProcess(reviewInfo _word, QObject *parent)
    : QObject{parent}
    , wordInfo{_word}
    , p{new QProcess(this)}
{}

void ClassifyProcess::start() {
    QStringList args;
    args << wordInfo.word;
    bool success = connect(p, SIGNAL(finished(int)), this, SLOT(endProcess(int)), Qt::QueuedConnection);
    Q_ASSERT(success);
    success = connect(p, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(handleError(QProcess::ProcessError)), Qt::QueuedConnection);
    Q_ASSERT(success);
//    success = connect(p, &QProcess::stateChanged, this, &ClassifyProcess::handleStateChange, Qt::QueuedConnection);
//    Q_ASSERT(success);
    p->start(PROCESS_LOC, args);
    p->waitForStarted();
}

void ClassifyProcess::endProcess(int exitCode) {
    emit finished(exitCode, wordInfo);
    p->close();
    p->deleteLater();
}

void ClassifyProcess::handleError(QProcess::ProcessError err)
{
    qDebug() << "QProcess error: " << err;
    emit finished(-1, wordInfo);
    p->close();
    p->deleteLater();
}

//void ClassifyProcess::handleStateChange(QProcess::ProcessState ps)
//{
//    qDebug() << "Process state:" << ps;
//}
