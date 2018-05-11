#ifndef TMU_H
#define TMU_H
#include <QObject>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMutex>
#include <math.h>
#include "utils.h"
#include "hxusbcomm.h"
#include "hx_commstd.h"
#include "tmu_std.h"
#include "tmu_gui_properties.h"
#include "tmutempcycle.h"

typedef unsigned char TmuTxPkt[NUM_OF_LATCHES][SIZE_OF_TX_LATCH_PKT];
typedef unsigned char TmuRxPkt[NUM_OF_LATCHES][SIZE_OF_RX_LATCH_PKT];



// Combo Box Option
struct cbOption {
    QString name;
    QString unit;
    uchar code;
};

struct latch
{
    QString name;
    uchar addr;
    uchar txPkt[SIZE_OF_TX_LATCH_PKT];
    uchar rxPkt[SIZE_OF_RX_LATCH_PKT];
};



struct Thermistor {
    double resRef;
    unsigned int beta;
    double resTarget;
    double tempRef;
    double tempTarget;
};

struct ThermalParams {
    double resAmb;
    double resMat;
    double capAmb;
};

class TMU
{
public:
    TMU();

    latch* latches[NUM_OF_LATCHES];
    latch adc_mux_latch;
    latch asic_rev_sfr;
    QMutex mutex;

    // Analog 0 latch
    bool setEHTransistor(bool nmosOrNpn);
    bool setEHResVal(double val); // no latch
    bool setEHResCount(unsigned int count); // no latch
    bool calcThermistorInfo();
    double calcResTarget();
    double calcResTarget(double TRef, double TTarget, unsigned int beta, double RRef);
    void setThermRRef(double rRef);
    void setThermTTarget(double tTarget);
    void setThermTRef(double tRef);
    void setThermBeta(unsigned int beta);
    Thermistor getThermInfo();

    // Oven Controller Latch
    bool setEnableOvenCtrl(bool en);
    bool setRangeRes(int index);
    bool setGainRes(int index);
    bool setIntOrExtHeater(bool intOrExt);
    bool setCurrLimit(double cl);
    void setCurrLimit(uchar dacCode);
    void calcAndSetCurrLimitVars(double cl);

    // Heat Latch
    bool setCelsius(double val);
    void setCelsius(unsigned int dacCode);

    // Bias Latch
    void setEnableVref(bool en);

    // CMOS Out Latch
    // void initCmosOutLatch();

    // Theraml Parameters
    void setAmbientRes(double ra);
    void setMaterialRes(double rm);
    void setAmbientCap(double ca);

    // MUX
    void setMuxChannel(unsigned int index);
    void setMuxChannelVal(unsigned int val) {muxChannelValue = val;}

    // ASIC REV
    void setAsicRev(uchar ar) { asic_rev = ar;}

    // GETTERS
    double getCurrLimit() { return currLimitVal;}
    double getCurrLimitVolt() { return currLimitVolt;}
    double getCurrLimitEH() { return currLimitEH;}
    unsigned int getMuxChannelVal() { return muxChannelValue;}
    void getMuxChannelVals(double& volt, unsigned int& adcCode, unsigned int& hex);
    uchar getAsicRev() { return asic_rev;}

    // Utility
    QStringList getRangeNames();
    QStringList getGainResNames();
    QStringList getMuxChannelNames();
    QStringList getMuxChannelUnits();
    QStringList getLatchNames();
    QStringList getLatchValQStrLst();

    // PACKET BUILDING
    void getWriteRegPkt(TmuTxPkt& tmuPkt);
    void getReadRegPkt(TmuRxPkt& tmuPkt);
    void getWriteOTPPkt(TmuTxPkt& tmuPkt);
    void getReadOTPPkt(TmuRxPkt& tmuPkt);
    void generateTxPkt(TmuTxPkt& tmuPkt, uchar cmd);
    void generateRxPkt(TmuRxPkt& tmuPkt, uchar cmd);



private:

    uchar asic_rev = 0;

    // Latches
    latch analog_0_latch;
    latch oven_ctlr_latch;
    latch heat_latch;
    latch bias_latch;
    latch cmos_out_latch;


    bool intOrExt;

    double currLimitVal;
    double currLimitVolt;
    uchar currLimitCode;

    double tempCelsiusVal;
    unsigned int tempCelsiusCode;

    uchar muxChannel;
    unsigned int muxChannelValue; // what is read from chip

    unsigned int numOfEHLegs;
    double valOfEHLeg ;
    double currLimitEH;
    Thermistor therm;

    ThermalParams tParams;

    cbOption* gainResistors[NUM_OF_GAIN_RES];
    cbOption* rangeResistors[NUM_OF_RANGE_RES];
    cbOption* muxChannels[NUM_OF_MUX_CHANNELS];

    // TEMP CYCLE
    TMUTempCycle* ttCycle;




    // init methods
    void initLatches();
    void initAnalog0Latch();
    void initOvenCtrlLatch();
    void initHeatLatch();
    void initBiasLatch();
    void initCmosOutLatch();
    void initAdcMuxLatch();
    void initAsicRevSFR();
    void initThermistor();
    void initGainResArr();
    void initRangeResArr();
    void initAdcChannelArr();


    //COMM.
    HxUSBComm *hxUSBComm;

};

#endif // TMU_H
