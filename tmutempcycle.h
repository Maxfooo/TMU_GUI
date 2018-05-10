#ifndef TMUTEMPCYCLE_H
#define TMUTEMPCYCLE_H

#include <QDebug>
#include <QFile>
#include <QtXml>
#include <QString>
#include "tmu_std.h"
#include "tmu_gui_properties.h"

extern QString CONFIG_FILE_PREFIX;
const QString GENERIC_PROFILE_FILE_NAME = "Temperature_Profile_0.xml";



struct TTData {
    double temp;
    unsigned int time;
};

class TMUTempCycle
{
public:
    TMUTempCycle();
    ~TMUTempCycle();


    QString getErrorStr() { return errStr;}

    void debugExport();
    void debugExportImport();

    bool setSawtooth(QString fname, double tStart, double tStop, double period);
    void setPiecewise(QString fname, double* temp, int tempSize, double* time, int timeSize);
    void setPiecewise(QString fname, TTData* ttdata, int ttdataSize);
    void setPiecewise(QString fname, const std::vector<double>& temp, const std::vector<double>& time);
    void setPiecewise(QString fname, const std::vector<TTData>& ttdata);

private:
    TTData tempProfile[PIECEWISE_STEP_COUNT];
    QString profileFileName = GENERIC_PROFILE_FILE_NAME;
    QString errStr = "";

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
