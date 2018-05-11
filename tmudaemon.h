#ifndef TMUDAEMON_H
#define TMUDAEMON_H

#include <QTimer>
#include "tmu.h"
#include "tmu_std.h"


class TMUDaemon
{
public:
    TMUDaemon(TMU* tmu);
    ~TMUDaemon();
    void start();
    void stop();

public slots:
    void checkTemperature();

private:
    TMU* tmu;
    QTimer* checkTimer;
};


#endif // TMUDAEMON_H
