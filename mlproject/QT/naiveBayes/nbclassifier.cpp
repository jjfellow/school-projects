#include "nbclassifier.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QThread>
#include <QSettings>

NBClassifier::NBClassifier(QObject *parent)
    : QObject{parent}
    , db{nullptr}
    , infoMap{QMap<QString, wordInfo>()}
    , word2ExitCode{QHash<QString, int>()}
    , five{0}
    , one{0}
    , processesStarted{0}
    , processesFinished{0}
    , processesRunning{0}
    , run{true}
{
    file.setFileName(FILE_NAME);
    QSettings settings(ORG_NAME, APP_NAME);
    global_count = settings.value("global_count", 0).toULongLong();

    if(global_count != 0) {
        // load progress saved
        QFile progress(SAVE_CSV);
        if(!progress.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open expected progress file.";
        }
        else {
            progress.readLine();
            while(!progress.atEnd()) {
                QStringList infoLine = QString(progress.readLine()).split(';');
                wordInfo wi;
                wi.postProb5 = infoLine[1].toDouble();
                wi.num1reviews = infoLine[2].toInt();
                wi.num5reviews = infoLine[3].toInt();
                infoMap[infoLine[0]] = wi;
            }
        }
    }
}

NBClassifier::~NBClassifier() {
    file.close();
}

void NBClassifier::setDB(DBManager * database)
{
    db = database;
}

void NBClassifier::saveProgress()
{
    run = false;
    while(processesRunning != 0) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    mutex.tryLock(990);
    savePrivate();
    QSettings settings(ORG_NAME, APP_NAME);
    settings.setValue("global_count", global_count);
    mutex.unlock();
    emit execDone();
}

void NBClassifier::savePrivate() {
    QFile out;
    out.setFileName(SAVE_CSV);
    out.open(QIODevice::WriteOnly);
    QTextStream output(&out);
    output << "WORD;POST_PROB_5;NUM_1_REVIEWS;NUM_5_REVIEWS\n";
    for(const auto& w: infoMap.keys()) {
        output << w << ";"
               << QString::number(infoMap[w].postProb5, 'f', 7) << ";"
               << QString::number(infoMap[w].num1reviews) << ";"
               << QString::number(infoMap[w].num5reviews)
               << "\n";
    }
    out.close();
}

void NBClassifier::exec() {
    if(db != nullptr && !(db->errors())) {
        qDebug() << "Executing from database";
        execDB();
    }
    else {
        qDebug() << "Executring from file";
        execF();
    }
}

void NBClassifier::execF() {
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    quint64 count = 0;
    int percent = 0;

    qDebug() << "Organizing words...";
    while(!in.atEnd() && run)
    {
        count++;
        qDebug() << count;
        if(count >= 150) {
//            percent++;
//            qDebug() << percent << "% done.";
//            count = 0;
            break;
        }
        QString val = in.readLine();
        QJsonParseError err;
        QJsonDocument r = QJsonDocument::fromJson(val.toLatin1(), &err);
        if(err.error != QJsonParseError::NoError)
        {
            qDebug() << err.errorString();
            continue;
        }
        int stars = r["rating"].toInt();
        if(stars == 1 || stars == 5) {
            QString review = r["text"].toString().toLower();
            review.remove(QRegExp(QString::fromUtf8("[-`~!@#$%^&*()_â€”+=|:;<>Â«Â»,.?/{}\'\"\\\[\\\]\\\\]")));
            QStringList tokens = review.split(' ');
            for(const auto& word : tokens) {
                if(word == "") continue;
                if(word2ExitCode.contains(word)) {
                    mutex.tryLock(10000);
                    if(word2ExitCode[word]) {
                        if(stars == 1)  infoMap[word].num1reviews++;
                        else            infoMap[word].num5reviews++;
                    }
                    // else do nothing
                    mutex.unlock();
                }
                else {
                    while(processesRunning >= MAX_PARALLEL_PROCESS) {
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                    }

                    mutex.tryLock(10000);
                    reviewInfo ri;
                    ri.word = word;
                    ri.stars = stars;
                    ClassifyProcess * cp = new ClassifyProcess(ri,this);
                    bool good = connect(cp, SIGNAL(finished(int,reviewInfo)), this, SLOT(endProcess(int,reviewInfo)), Qt::QueuedConnection);
                    Q_ASSERT(good);
                    processesStarted++;
                    processesRunning++;
                    mutex.unlock();
                    cp->start();
                }
            }
        }
    }

    if(run) {
        while(processesStarted != processesFinished) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        qDebug() << "100 % done organizing words.";
        qDebug() << processesFinished << processesStarted;
        qDebug() << "Performing calculations...";

        quint64 total = five + one;
        double prob_5 = (double)five/(double)total;
        for(const auto& w : infoMap.keys()) {
            wordInfo * wi = &infoMap[w];
            int total_word = wi->num1reviews + wi->num5reviews;
            wi->postProb5 = (((double)wi->num5reviews/(double)five) * prob_5) / ((double)total_word/(double)total);
        }

        qDebug() << "Calculations done.";
        qDebug() << "Writing out to file...";

        savePrivate();

        qDebug() << "All done.";
        emit execDone();
    }
}

void NBClassifier::execDB() {
    int count = 0;
    int fiftyCount = 0;
    int totalReviewsover10000 = db->numberOfReviews()/10000;
    qDebug() << totalReviewsover10000;
    double percent = 0;

    if(global_count > 0) {
        // fast forward through database
        db->fastForward(global_count);
        int temp_count = global_count;
        while(temp_count > totalReviewsover10000) {
            temp_count -= totalReviewsover10000;
            percent +=0.01;
        }
        count = temp_count;
    }

    review currentReview = db->nextReview();

    qDebug() << "Organizing words...";
    while(!(currentReview.stars == -1) && run)
    {
        count++;
        fiftyCount++;
        global_count++;
        qDebug() << count;
        if(count >= totalReviewsover10000) {
            percent += 0.01;
            qDebug() << percent << "% done.";
            count = 0;
        }
        if(fiftyCount >= 50) {
            mutex.tryLock(990);
            savePrivate();
            QSettings settings(ORG_NAME, APP_NAME);
            settings.setValue("global_count", global_count);
            fiftyCount = 0;
            mutex.unlock();
        }
        int stars = currentReview.stars;
        if(stars == 1 || stars == 5) {
            QString review = currentReview.text.toLower();
            review.remove(QRegExp(QString::fromUtf8("[-`~!@#$%^&*()_â€”+=|:;<>Â«Â»,.?/{}\'\"\\\[\\\]\\\\]")));
            QStringList tokens = review.split(' ');
            for(const auto& word : tokens) {
                if(word == "") continue;
                if(word2ExitCode.contains(word)) {
                    mutex.tryLock(10000);
                    if(word2ExitCode[word]) {
                        if(stars == 1)  infoMap[word].num1reviews++;
                        else            infoMap[word].num5reviews++;
                    }
                    // else do nothing
                    mutex.unlock();
                }
                else {
                    while(processesRunning >= MAX_PARALLEL_PROCESS) {
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                    }

                    mutex.tryLock(10000);
                    reviewInfo ri;
                    ri.word = word;
                    ri.stars = stars;
                    ClassifyProcess * cp = new ClassifyProcess(ri,this);
                    bool good = connect(cp, SIGNAL(finished(int,reviewInfo)), this, SLOT(endProcess(int,reviewInfo)), Qt::QueuedConnection);
                    Q_ASSERT(good);
                    processesStarted++;
                    processesRunning++;
                    mutex.unlock();
                    cp->start();
                }
            }
        }
        currentReview = db->nextReview();
    }

    if(run) {
        if(db->errors()){
            qDebug() << db->lastError();
        }

        while(processesStarted != processesFinished) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        qDebug() << "100 % done organizing words.";
        qDebug() << processesFinished << processesStarted;
        qDebug() << "Performing calculations...";

        quint64 total = five + one;
        double prob_5 = (double)five/(double)total;
        for(const auto& w : infoMap.keys()) {
            wordInfo * wi = &infoMap[w];
            int total_word = wi->num1reviews + wi->num5reviews;
            wi->postProb5 = (((double)wi->num5reviews/(double)five) * prob_5) / ((double)total_word/(double)total);
        }

        qDebug() << "Calculations done.";
        qDebug() << "Writing out to file...";

        savePrivate();

        qDebug() << "All done.";
        emit execDone();
    }
}

QJsonObject NBClassifier::ObjectFromString(const QString& in)
{
    QJsonValue val(in);
    return val.toObject();
}

void NBClassifier::endProcess(int exitCode, reviewInfo ri)
{
    mutex.tryLock(10000);

    word2ExitCode[ri.word] = exitCode;

    if(exitCode)
    {
        int fivestars=0, onestar=0;
        if(ri.stars == 1) {
            onestar = 1;
            one++;
        }
        else {
            fivestars = 1;
            five++;
        }

        if(infoMap.contains(ri.word)) {
            wordInfo * wi = &infoMap[ri.word];
            wi->num1reviews += onestar;
            wi->num5reviews += fivestars;
        }
        else {
            wordInfo wi;
            wi.num1reviews = onestar;
            wi.num5reviews = fivestars;
            infoMap[ri.word] = wi;
        }
    }
    processesFinished++;
    processesRunning--;
    mutex.unlock();
}


