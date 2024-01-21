#include <QCoreApplication>
#include <QTimer>

#include <initializer_list>
#include <signal.h>
#include <unistd.h>

#include "nbclassifier.h"
#include "dbmanager.h"

static NBClassifier * nbc;

void catchUnixSignals(std::initializer_list<int> quitSignals) {
    auto handler = [](int sig) -> void {
        nbc->saveProgress();
        printf("\nquit the application by signal(%d).\n", sig);
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    qRegisterMetaType<reviewInfo>();
    qRegisterMetaType<QProcess::ProcessError>();
    qRegisterMetaType<QProcess::ProcessState>();
    // Task parented to the application so that it
    // will be deleted by the application.
    nbc = new NBClassifier(&a);
    DBManager db("/home/kate/school/mlearning/Project/companyReviews.db", &a);
    if(!db.errors()) {
        nbc->setDB(&db);
    }

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(nbc, SIGNAL(execDone()), &a, SLOT(quit()));

    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});

    // This will run the task from the application event loop.
    QTimer::singleShot(0, nbc, SLOT(exec()));

    return a.exec();
}
