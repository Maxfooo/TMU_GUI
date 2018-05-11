#ifndef TMUTEMPCYCLE_H
#define TMUTEMPCYCLE_H

#include <QDebug>
#include <QFile>
#include <QtXml>
#include <QString>
#include <QTimer>
#include <QObject>
#include "tmu_std.h"
#include "tmu_gui_properties.h"
#include "tmu.h"


extern QString CONFIG_FILE_PREFIX;
const QString GENERIC_PROFILE_FILE_NAME = "Temperature_Profile_0.xml";



struct TTData {
    double temp;
    unsigned int time;
};

class TMUTempCycle : public QObject
{
    Q_OBJECT
public:
    TMUTempCycle(uchar id, TMU *tmu);
    ~TMUTempCycle();

    QString getErrorStr() { return errStr;}
    void setFileName(QString fname) {profileFileName = fname;}
    uchar getID() {return this->id;}

    void debugExport();
    void debugExportImport();

    bool setSawtooth(double tStart, double tStop, double period);
    bool setPiecewise(double* temp, int tempSize, double* time, int timeSize);
    bool setPiecewise(TTData* ttdata, int ttdataSize);
    bool setPiecewise(const std::vector<double>& temp, const std::vector<double>& time);
    bool setPiecewise(const std::vector<TTData>& ttdata);
    bool isReady() { return tmuTempCycleReady;}
    void start();
    void pause();
    void resume();
    void stop();

public slots:
    void updateHeatLatch();

private:
    TMU* tmu;
    uchar id;
    QTimer* checkTimer;
    TTData tempProfile[PIECEWISE_STEP_COUNT];
    QString profileFileName = GENERIC_PROFILE_FILE_NAME;
    QString errStr = "";
    bool tmuTempCycleReady = false;

    const QString XML_ROOT = "Temperature_Profile";
    const QString XML_TEMP = "Temperature";
    QString XML_TEMP_N = "C%1";
    const QString XML_TIME = "Time";
    QString XML_TIME_N = "S%1";

    bool importTempProf(QString fname);
    bool importTempProf();

    bool exportTempProf(QString fname);
    bool exportTempProf();

};


#endif // TMUTEMPCYCLE_H
