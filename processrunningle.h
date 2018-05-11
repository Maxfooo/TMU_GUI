#ifndef PROCESSRUNNINGLE_H
#define PROCESSRUNNINGLE_H

#include <QDebug>
#include <QString>
#include <QLineEdit>
#include <QMutex>
#include <QTimer>
#include <QObject>
#include <QMainWindow>
#include <vector>

class ProcessRunningLE : public QObject
{
    Q_OBJECT
public:
    ProcessRunningLE(QLineEdit* procRunLE);
    ProcessRunningLE(QLineEdit* procRunLE, double period);
    ~ProcessRunningLE();
    void setPeriod(double period);
    void startProcRun();
    void pauseProcRun();
    void resumeProcRun();
    void stopProcRun();

public slots:
    void updateRunProcLE();

private:
    QMainWindow* parent;
    QLineEdit* procRunLE;
    double period = 1;
    unsigned int leLength;
    unsigned int sliderLength;
    double timeStep;

    std::vector<QString> procRunText;

    QTimer* procRunTimer;

    void initProcRunText();
    void initProcRunLE();
    QString getLEText();

    template <class T>
    std::vector<T> rotVect(bool toRight, std::vector<T> v, unsigned int rot);
};

#endif // PROCESSRUNNINGLE_H
