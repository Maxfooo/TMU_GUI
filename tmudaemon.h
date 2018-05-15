#ifndef TMUDAEMON_H
#define TMUDAEMON_H

#include <QTimer>
#include <QObject>
#include "tmu.h"
#include "tmu_std.h"


class TMUDaemon : public QObject
{
    Q_OBJECT
public:
    TMUDaemon(uchar id, TMU* tmu);
    ~TMUDaemon();
    void start();
    void stop();
    void setID(uchar id) { this->id = id;}
    uchar getID() {return this->id;}

signals:
    void rxTemperature(uchar id);

public slots:
    void checkTemperature();

private:
    TMU* tmu;
    QTimer* checkTimer;
    uchar id;
};


#endif // TMUDAEMON_H
