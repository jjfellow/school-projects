#ifndef CLASSIFYPROCESS_H
#define CLASSIFYPROCESS_H

#include <QObject>
#include <QProcess>

typedef struct ri {
    int stars;
    QString word;
}reviewInfo;

class ClassifyProcess : public QObject
{
    Q_OBJECT
public:
    explicit ClassifyProcess(reviewInfo _word, QObject *parent = nullptr);
    ClassifyProcess(const ClassifyProcess&);
    reviewInfo wordInfo;
    void start();

    QProcess * p;

private:
    const QString PROCESS_LOC = "../dist/dictCheck/dictCheck";

public slots:
    void endProcess(int exitCode);
    void handleError(QProcess::ProcessError);
//    void handleStateChange(QProcess::ProcessState);

signals:
    void finished(int exitCode, reviewInfo info);
};

Q_DECLARE_METATYPE(reviewInfo)

#endif // CLASSIFYPROCESS_H
