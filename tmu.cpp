#include "tmu.h"

// Range Resistors
cbOption rr7 = {"2.7", "kOhms", 0x00};
cbOption rr6 = {"3.2", "kOhms", 0x01};
cbOption rr5 = {"4.4", "kOhms", 0x02};
cbOption rr4 = {"5.0", "kOhms", 0x03};
cbOption rr3 = {"5.8", "kOhms", 0x04};
cbOption rr2 = {"6.9", "kOhms", 0x05};
cbOption rr1 = {"0.0", "Ohms", 0x06};
cbOption rr0 = {"0.0", "Ohms", 0x07};

// Gain Resistors (In the feedback path of heater leg to opamp)
cbOption gr7 = {"0.0", "Ohms", 0x00};
cbOption gr6 = {"200", "Ohms", 0x01};
cbOption gr5 = {"500", "Ohms", 0x02};
cbOption gr4 = {"1.0", "kOhms", 0x03};
cbOption gr3 = {"2.5", "kOhms", 0x04};
cbOption gr2 = {"5.0", "kOhms", 0x05};
cbOption gr1 = {"7.5", "kOhms", 0x06};
cbOption gr0 = {"10.0", "kOhms", 0x07};

cbOption m0 = {"Ch0 THRM1", "Volts", 0x00};
cbOption m1 = {"Ch2 EXFB", "Volts", 0x02};
cbOption m2 = {"Ch3 INTFB1", "Volts", 0x03};
cbOption m3 = {"Ch4 INTFB2", "Volts", 0x04};
cbOption m4 = {"Ch5 TEMPSENSE", "Volts", 0x05};
cbOption m5 = {"Ch6 TDRV", "Volts", 0x06};
cbOption m6 = {"Ch11 TEMPDAC", "Volts", 0x0B};
cbOption m7 = {"Ch12 LIMITDAC", "Volts", 0x0C};

TMU::TMU()
{
    intOrExt = true; // Internal

    numOfEHLegs = 1;
    valOfEHLeg = 2;
    currLimitEH = CURR_LIMIT_DAC_VOLT_MAX/valOfEHLeg*1000;

    initLatches();
    initThermistor();
    initGainResArr();
    initRangeResArr();
    initAdcChannelArr();
}

void TMU::initLatches()
{
    // Order is arbitrary
    latches[0] = &analog_0_latch;
    latches[1] = &oven_ctlr_latch;
    latches[2] = &heat_latch;
    latches[3] = &bias_latch;
    latches[4] = &cmos_out_latch;
    // adc_mux_latch and asic_rev_sfr stays independent

    initAnalog0Latch();
    initOvenCtrlLatch();
    initHeatLatch();
    initBiasLatch();
    initCmosOutLatch();

    initAdcMuxLatch();
    initAsicRevSFR();
}

void TMU::initAdcMuxLatch()
{
    adc_mux_latch.name = "ADC_MUX";
    adc_mux_latch.addr = ADC_MUX_ADDR;
    adc_mux_latch.txPkt[CMD_INDEX] = CMD_WRITE_LATCH;
    adc_mux_latch.txPkt[VAL_HI_INDEX] = 0x00;
    adc_mux_latch.txPkt[VAL_LO_INDEX] = 0x00;
    adc_mux_latch.txPkt[TX_LAT_NUM_INDEX] = ADC_MUX_NUM;
    adc_mux_latch.txPkt[TX_CS_INDEX] = 0x00;
    adc_mux_latch.rxPkt[CMD_INDEX] = CMD_READ_LATCH;
    adc_mux_latch.rxPkt[RX_LAT_NUM_INDEX] = ADC_MUX_NUM;
    adc_mux_latch.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(adc_mux_latch.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

void TMU::initAsicRevSFR()
{
    asic_rev_sfr.name = "ASIC_REV";
    asic_rev_sfr.addr = ASIC_REV_ADDR;
    asic_rev_sfr.txPkt[CMD_INDEX] = 0x00;
    asic_rev_sfr.txPkt[VAL_HI_INDEX] = 0x00;
    asic_rev_sfr.txPkt[VAL_LO_INDEX] = 0x00;
    asic_rev_sfr.txPkt[TX_LAT_NUM_INDEX] = 0x00;
    asic_rev_sfr.txPkt[TX_CS_INDEX] = 0x00;
    asic_rev_sfr.rxPkt[CMD_INDEX] = CMD_READ_SFR;
    asic_rev_sfr.rxPkt[RX_LAT_NUM_INDEX] = ASIC_REV_ADDR;
    asic_rev_sfr.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(asic_rev_sfr.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

void TMU::initThermistor()
{
    // Init using internal heater parameters

    therm.resRef = THERM_NOM_RES_REF;
    therm.beta = THERM_NOM_BETA;
    therm.tempRef = 25;

    therm.tempTarget = THERM_NOM_SET_TEMP;

    therm.resTarget = this->calcResTarget(therm.tempRef, therm.tempTarget,
                                          therm.beta, therm.resRef);
    #ifdef DEBUG
        qDebug() << QString("Init resTarget = %1").arg(therm.resTarget);
    #endif

}

void TMU::initRangeResArr()
{
    rangeResistors[0] = &rr0;
    rangeResistors[1] = &rr1;
    rangeResistors[2] = &rr2;
    rangeResistors[3] = &rr3;
    rangeResistors[4] = &rr4;
    rangeResistors[5] = &rr5;
    rangeResistors[6] = &rr6;
    rangeResistors[7] = &rr7;

}

void TMU::initGainResArr()
{
    gainResistors[0] = &gr0;
    gainResistors[1] = &gr1;
    gainResistors[2] = &gr2;
    gainResistors[3] = &gr3;
    gainResistors[4] = &gr4;
    gainResistors[5] = &gr5;
    gainResistors[6] = &gr6;
    gainResistors[7] = &gr7;
}

void TMU::initAdcChannelArr()
{
    muxChannels[0] = &m0;
    muxChannels[1] = &m1;
    muxChannels[2] = &m2;
    muxChannels[3] = &m3;
    muxChannels[4] = &m4;
    muxChannels[5] = &m5;
    muxChannels[6] = &m6;
    muxChannels[7] = &m7;
}


/************************/
// Analog 0 latch
// Bit   - Name
// <1:0> - N/A
// <4:2> - N/A
// <5>   - N/A
// <6>   - Oven Controller Maximum Drive Current (NPN(0)/NMOS(1))
// <7>   - N/A
/************************/
void TMU::initAnalog0Latch()
{
    analog_0_latch.name = "ANALOG_0";
    analog_0_latch.addr = ANALOG_0_ADDR;
    analog_0_latch.txPkt[CMD_INDEX] = CMD_WRITE_LATCH;
    analog_0_latch.txPkt[VAL_HI_INDEX] = 0x00;
    analog_0_latch.txPkt[VAL_LO_INDEX] = 0x00;
    analog_0_latch.txPkt[TX_LAT_NUM_INDEX] = ANALOG_0_NUM;
    analog_0_latch.txPkt[TX_CS_INDEX] = 0x00;

    analog_0_latch.rxPkt[CMD_INDEX] = CMD_READ_LATCH;
    analog_0_latch.rxPkt[RX_LAT_NUM_INDEX] = ANALOG_0_NUM;
    analog_0_latch.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(analog_0_latch.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

bool TMU::setEHTransistor(bool nmosOrNpn)
{
    if (nmosOrNpn)
    {
        analog_0_latch.txPkt[VAL_LO_INDEX] |= 0x40; // 0100 0000
        #ifdef DEBUG
            qDebug() << QString("Analog 0 latch = %1").arg(QString::number(analog_0_latch.txPkt[VAL_LO_INDEX], 16));
        #endif
        return true;
    }
    analog_0_latch.txPkt[VAL_LO_INDEX] &= 0xBF; // 1011 1111
    #ifdef DEBUG
        qDebug() << QString("Analog 0 latch = %1").arg(QString::number(analog_0_latch.txPkt[VAL_LO_INDEX], 16));
    #endif
    return true;
}

bool TMU::setEHResVal(double val) // no latch
{
    valOfEHLeg = val;
    currLimitEH = CURR_LIMIT_DAC_VOLT_MAX/valOfEHLeg*1000;
    #ifdef DEBUG
        qDebug() << QString("External Heater resistor value = %1").arg(QString::number(val));
    #endif
}

bool TMU::setEHResCount(unsigned int count) // no latch
{
    numOfEHLegs = count;
    #ifdef DEBUG
        qDebug() << QString("Number of External Heaters resistors = %1").arg(QString::number(count));
    #endif
}

bool TMU::calcThermistorInfo()
{


}

double TMU::calcResTarget(double TRef, double TTarget, unsigned int beta, double RRef)
{
    // C_TO_K = 273.15
    // Beta Equation: https://en.wikipedia.org/wiki/Thermistor
    double expo1 =  1/(C_TO_K + TRef);
    double expo2 =  1/(C_TO_K + TTarget);
    double expo3 = beta*(expo1-expo2);
    return ( RRef*exp(-1*expo3) );
}

double TMU::calcResTarget()
{
    therm.resTarget = this->calcResTarget(therm.tempRef, therm.tempTarget, therm.beta, therm.resRef);
    return therm.resTarget;
}

void TMU::setThermRRef(double rRef)
{
    // rRef is given in kOhms
    therm.resRef = rRef*1000;
}

void TMU::setThermTTarget(double tTarget)
{
    therm.tempTarget = tTarget;
}

void TMU::setThermTRef(double tRef)
{
    therm.tempRef = tRef;
}

void TMU::setThermBeta(unsigned int beta)
{
    therm.beta = beta;
}

Thermistor TMU::getThermInfo()
{
    return therm;
}

/************************/
// Oven Controller Latch
// Bit    - Name
// <2:0>  - Oven Controller Feedback Gain
// <3>    - Enable Oven Controller
// <6:4>  - Thermistor Range Set
// <7>    - Internal/External Controller Select (1=Internal)
// <8>    - Heat Drive Opamp (Temp Control DAC) Enable
// <15:9> - Current Limit DAC
/************************/
void TMU::initOvenCtrlLatch()
{
    oven_ctlr_latch.name = "OVEN_CTLR";
    oven_ctlr_latch.addr = OVEN_CTLR_ADDR;
    oven_ctlr_latch.txPkt[CMD_INDEX] = CMD_WRITE_LATCH;
    oven_ctlr_latch.txPkt[VAL_HI_INDEX] = 0x01;
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] = 0x88;
    oven_ctlr_latch.txPkt[TX_LAT_NUM_INDEX] = OVEN_CTLR_NUM;
    oven_ctlr_latch.txPkt[TX_CS_INDEX] = 0x00;

    oven_ctlr_latch.rxPkt[CMD_INDEX] = CMD_READ_LATCH;
    oven_ctlr_latch.rxPkt[RX_LAT_NUM_INDEX] = OVEN_CTLR_NUM;
    oven_ctlr_latch.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(oven_ctlr_latch.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

bool TMU::setEnableOvenCtrl(bool en)
{
    if (en)
    {
        oven_ctlr_latch.txPkt[VAL_LO_INDEX] |= 0x04; // 0000 0100
        #ifdef DEBUG
            qDebug() << QString("Enable Oven Controller; Oven Controller Latch = %1").arg(QString::number(oven_ctlr_latch.txPkt[VAL_LO_INDEX], 16));
        #endif
        return true;
    }
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] &= 0xFB; // 1111 1011
    #ifdef DEBUG
        qDebug() << QString("Disable Oven Controller; Oven Controller Latch = %1").arg(QString::number(oven_ctlr_latch.txPkt[VAL_LO_INDEX], 16));
    #endif
    return true;

}

bool TMU::setRangeRes(int index)
{
    // Thermistor range in datasheet
    // It is expected the index matches the datasheet

    // set bits 4,5,6 to 0
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] &= 0x8F; // 1000 1111

    // mask then shift index to bits 4,5,6
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] |= ( (0x07 & index) << 4);

    #ifdef DEBUG
        qDebug() << QString("Gain Resistance set to index %1; Oven Controller Latch = %2").arg(QString::number(index)).arg(QString::number(oven_ctlr_latch.txPkt[VAL_LO_INDEX], 16));
    #endif

    return true;
}

bool TMU::setGainRes(int index)
{
    // Oven Cotnroller Feedback Gain in datasheet
    // It is expected the index matches the datasheet

    // set lower 3 bits to 0
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] &= 0xF8; // 1111 1000

    // mask then write index to lower 3 bits
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] |= (0x07 & index);

    #ifdef DEBUG
        qDebug() << QString("Feedback Resistance set to index %1; Oven Controller Latch = %2").arg(QString::number(index)).arg(QString::number(oven_ctlr_latch.txPkt[VAL_LO_INDEX], 16));
    #endif

    return true;
}

bool TMU::setIntOrExtHeater(bool intOrExt)
{
    oven_ctlr_latch.txPkt[VAL_LO_INDEX] = intOrExt ? oven_ctlr_latch.txPkt[VAL_LO_INDEX] | 0x80 :
                                       oven_ctlr_latch.txPkt[VAL_LO_INDEX] & 0x7F;
    this->intOrExt = intOrExt;

    #ifdef DEBUG
        QString heater = "";
        heater = intOrExt ? "Internal" : "External";
        qDebug() << QString("Setting Heater to %1; Oven Controller Latch = %2").arg(heater).arg(QString::number(oven_ctlr_latch.txPkt[VAL_LO_INDEX], 16));
    #endif
    return true;
}

bool TMU::setCurrLimit(double cl)
{

    if (intOrExt)
    {
        if (cl > CURR_LIMIT_CHIP_MAX)
        {
            #ifdef DEBUG
                qDebug() << QString("Internal Current Limit = %1").arg(QString::number(CURR_LIMIT_CHIP_MAX));
            #endif
            calcAndSetCurrLimitVars(CURR_LIMIT_CHIP_MAX);
            return false;
        }
        #ifdef DEBUG
            qDebug() << QString("Internal Current Limit = %1").arg(QString::number(cl));
        #endif
        calcAndSetCurrLimitVars(cl);
        return true;
    }
    else
    {
        /////// FILL IN ///////
        if (numOfEHLegs > 0 && valOfEHLeg > 0)
        {
            if (cl > currLimitEH)
            {
                calcAndSetCurrLimitVars(currLimitEH);
                return false;
            }
            calcAndSetCurrLimitVars(cl);
            return true;
        }
        // default values
        this->setEHResCount(1);
        this->setEHResVal(EXT_RES_VAL_MIN);
        calcAndSetCurrLimitVars(currLimitEH);
        return false;
    }
}

void TMU::calcAndSetCurrLimitVars(double cl)
{
    if (intOrExt)
    {
        currLimitVolt = CURR_LIMIT_V_TO_MA_SLOPE*cl/NUM_INT_HEATING_UNITS;
    }
    else
    {
        currLimitVolt = valOfEHLeg*cl/1000; // convert milliamps to amps
    }
    currLimitCode = static_cast<uchar>(std::round(currLimitVolt/CURR_LIMIT_DAC_LSB));
    this->setCurrLimit(currLimitCode);
    // old method
    // currLimitVolt = cl/NUM_INT_HEATING_UNITS/NUM_INT_HEATER_LEGS/100 * currLimitLegRes;
}

void TMU::setCurrLimit(uchar dacCode)
{
    // Set top 7 bits to 0
    oven_ctlr_latch.txPkt[VAL_HI_INDEX] &= 0x01;

    // mask and shift to top 7 bits
    oven_ctlr_latch.txPkt[VAL_HI_INDEX] |= ( (0xFE & dacCode) << 1);

    #ifdef DEBUG
        qDebug() << QString("Current Limit DAC Code = %1; Oven Controller Latch (valHi) = %2").arg(QString::number(dacCode, 2)).arg(QString::number(oven_ctlr_latch.txPkt[VAL_HI_INDEX], 16));
    #endif
}

/************************/
// Heat Latch
// Bit     - Name
// <11:0>  - Temperature Control DAC
// <15:12> - N/A
/************************/
void TMU::initHeatLatch()
{
    heat_latch.name = "HEAT";
    heat_latch.addr = HEAT_ADDR;
    heat_latch.txPkt[CMD_INDEX] = CMD_WRITE_LATCH;
    heat_latch.txPkt[VAL_HI_INDEX] = 0x80;
    heat_latch.txPkt[VAL_LO_INDEX] = 0x00;
    heat_latch.txPkt[TX_LAT_NUM_INDEX] = HEAT_NUM;
    heat_latch.txPkt[TX_CS_INDEX] = 0x00;

    heat_latch.rxPkt[CMD_INDEX] = CMD_READ_LATCH;
    heat_latch.rxPkt[RX_LAT_NUM_INDEX] = HEAT_NUM;
    heat_latch.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(heat_latch.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

bool TMU::setCelsius(double val)
{
    if (val > TEMP_CELSIUS_MAX)
    {
        tempCelsiusVal = TEMP_CELSIUS_MAX;
        tempCelsiusCode = static_cast<unsigned int>(std::round(tempCelsiusVal/TEMP_CELSIUS_LSB));
        #ifdef DEBUG
            qDebug() << QString("Temperature = %1°C").arg(QString::number(TEMP_CELSIUS_MAX));
        #endif
        this->setCelsius(tempCelsiusCode);
        return false;
    }
    else if (val < TEMP_CELSIUS_MIN)
    {
        tempCelsiusVal = TEMP_CELSIUS_MIN;
        tempCelsiusCode = static_cast<unsigned int>(std::round(tempCelsiusVal/TEMP_CELSIUS_LSB));
        #ifdef DEBUG
            qDebug() << QString("Temperature = %1°C").arg(QString::number(TEMP_CELSIUS_MIN));
        #endif
        this->setCelsius(tempCelsiusCode);
        return false;
    }
    tempCelsiusVal = val;
    tempCelsiusCode = static_cast<unsigned int>(std::round(tempCelsiusVal/TEMP_CELSIUS_LSB));
    #ifdef DEBUG
        qDebug() << QString("Temperature = %1°C").arg(QString::number(tempCelsiusVal));
    #endif
    this->setCelsius(tempCelsiusCode);
    return true;
}

void TMU::setCelsius(unsigned int dacCode)
{
    // reset latch value to 0
    heat_latch.txPkt[VAL_LO_INDEX] = 0x00;
    heat_latch.txPkt[VAL_HI_INDEX] = 0x00;

    // mask, shift, and set appropriately (12 bits)
    heat_latch.txPkt[VAL_LO_INDEX] = (0xFF & dacCode);
    heat_latch.txPkt[VAL_HI_INDEX] = (0x0F & (dacCode >> 8));

    #ifdef DEBUG
        qDebug() << QString("Temperature DAC Code = %1\nHeat Latch = %2 %3").arg(QString::number(dacCode, 2)).arg(QString::number(heat_latch.txPkt[VAL_HI_INDEX], 16)).arg(QString::number(heat_latch.txPkt[VAL_LO_INDEX], 16));
    #endif
}

/************************/
// Bias Latch
// Bit - Name
// <0> - Enable DAC Reference
// <1> - Enable ADC Reference
// <2> - Enable ZTC Current References
// <3> - Enable Oscillator Regulator
// <4> - Enable External Reference
// <15:5> - N/A
/************************/
void TMU::initBiasLatch()
{
    bias_latch.name = "BIAS";
    bias_latch.addr = BIAS_ADDR;
    bias_latch.txPkt[CMD_INDEX] = CMD_WRITE_LATCH;
    bias_latch.txPkt[VAL_HI_INDEX] = 0x00;
    bias_latch.txPkt[VAL_LO_INDEX] = 0x0F; // 000(0)1111 // (#) = user, init as 0
    bias_latch.txPkt[TX_LAT_NUM_INDEX] = BIAS_NUM;
    bias_latch.txPkt[TX_CS_INDEX] = 0x00;

    bias_latch.rxPkt[CMD_INDEX] = CMD_READ_LATCH;
    bias_latch.rxPkt[RX_LAT_NUM_INDEX] = BIAS_NUM;
    bias_latch.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(cmos_out_latch.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

void TMU::setEnableVref(bool en)
{
    if (en)
    {
        bias_latch.txPkt[VAL_LO_INDEX] = 0x1F;
        #ifdef DEBUG
            qDebug() << QString("Enable Vref; Bias Latch = %1 %2").arg(QString::number(bias_latch.txPkt[VAL_HI_INDEX], 16)).arg(QString::number(bias_latch.txPkt[VAL_LO_INDEX], 16));
        #endif
    }
    bias_latch.txPkt[VAL_LO_INDEX] = 0x0F;
    #ifdef DEBUG
        qDebug() << QString("Disable Vref; Bias Latch = %1 %2").arg(QString::number(bias_latch.txPkt[VAL_HI_INDEX], 16)).arg(QString::number(bias_latch.txPkt[VAL_LO_INDEX], 16));
    #endif
}

/************************/
// CMOS Out Latch
// Bit   - Name
// <0>   - Output Transition Speed (0 = fast)
// <1>   - EN Pullup/Pulldown Sense (0 = pulldown)
// <2>   - EN Polarity (0 = En Low)
// <3>   - Digital Test Mode (1 = Test Mode)
// <4>   - Resistor Sheet Measure Enable
// <7:5> - Test Signal (0=Idle)
/************************/
void TMU::initCmosOutLatch()
{
    cmos_out_latch.name = "CMOS_OUT";
    cmos_out_latch.addr = CMOS_OUT_ADDR;
    cmos_out_latch.txPkt[CMD_INDEX] = CMD_WRITE_LATCH;
    cmos_out_latch.txPkt[VAL_HI_INDEX] = 0x00;
    cmos_out_latch.txPkt[VAL_LO_INDEX] = 0x18;
    cmos_out_latch.txPkt[TX_LAT_NUM_INDEX] = CMOS_OUT_NUM;
    cmos_out_latch.txPkt[TX_CS_INDEX] = 0x00;

    cmos_out_latch.rxPkt[CMD_INDEX] = CMD_READ_LATCH;
    cmos_out_latch.rxPkt[RX_LAT_NUM_INDEX] = CMOS_OUT_NUM;
    cmos_out_latch.rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(cmos_out_latch.rxPkt, SIZE_OF_RX_LATCH_PKT-1);
}

/************************/
// THERMAL PARAMETERS
/************************/

void TMU::setAmbientRes(double ra) // given kOhms
{
    tParams.resAmb = ra*1000;
}

void TMU::setMaterialRes(double rm)
{
    tParams.resMat = rm;
}

void TMU::setAmbientCap(double ca) // given in milli-Farads
{
    tParams.capAmb = ca/1000;
}

/************************/
// MUX
/************************/
void TMU::setMuxChannel(unsigned int index)
{
    muxChannel = static_cast<uchar>(index);
    #ifdef DEBUG
        qDebug() << QString("Mux channel = %1").arg(QString::number(index));
    #endif
}

/************************/
// GETTERS
/************************/
void TMU::getMuxChannelVals(double& volt, unsigned int& adcCode, unsigned int& hex)
{
    // volt = muxChannelValue*MATH;
    // adcCode = muxChannelValue*MATH;
    // hex = muxChannelValue*MATH;
}

/************************/
// UTILITY
/************************/
QStringList TMU::getRangeNames()
{
    QStringList names;
    for (const auto& n: rangeResistors)
    {
        names << QString("%1 %2").arg(n->name, n->unit);
    }
    return names;
}

QStringList TMU::getGainResNames()
{
    QStringList names;
    for (const auto& n: gainResistors)
    {
        names << QString("%1 %2").arg(n->name, n->unit);
    }
    return names;
}

QStringList TMU::getMuxChannelNames()
{
    QStringList names;
    for (const auto& n: muxChannels)
    {
        names << n->name;
    }
    return names;
}

QStringList TMU::getMuxChannelUnits()
{
    QStringList units;
    for (const auto& u: muxChannels)
    {
        units << u->unit;
    }
    return units;
}

QStringList TMU::getLatchNames()
{
    QStringList latchNames;
    for (const auto& l: latches)
    {
        latchNames << l->name;
    }
    return latchNames;
}

QStringList TMU::getLatchValQStrLst()
{
    QStringList latchVals;
    unsigned int latchVal;
    for (const auto& l: latches)
    {
        latchVal = ((0x0000 | l->txPkt[VAL_HI_INDEX]) << 8) | (0x0000 | l->txPkt[VAL_LO_INDEX]);
        latchVals << QString::number(latchVal, 16);
    }
    return latchVals;
}

void TMU::getWriteRegPkt(TmuTxPkt& tmuPkt)
{
    this->generateTxPkt(tmuPkt, CMD_WRITE_LATCH);
}

void TMU::getReadRegPkt(TmuRxPkt& tmuPkt)
{
    this->generateRxPkt(tmuPkt, CMD_READ_LATCH);
}

void TMU::getWriteOTPPkt(TmuTxPkt& tmuPkt)
{
    this->generateTxPkt(tmuPkt, CMD_WRITE_OTP);
}

void TMU::getReadOTPPkt(TmuRxPkt& tmuPkt)
{
    this->generateRxPkt(tmuPkt, CMD_READ_OTP);
}

void TMU::generateTxPkt(TmuTxPkt& tmuPkt, uchar cmd)
{
    for (int i = 0; i < NUM_OF_LATCHES; i++)
    {
        latches[i]->txPkt[CMD_INDEX] = cmd;
        latches[i]->txPkt[TX_CS_INDEX] = HxUtils::calcChecksum(latches[i]->txPkt, SIZE_OF_TX_LATCH_PKT-1);
        for (int j = 0; j < SIZE_OF_TX_LATCH_PKT; j++)
        {
            tmuPkt[i][j] = latches[i]->txPkt[j];
        }
    }
}

void TMU::generateRxPkt(TmuRxPkt& tmuPkt, uchar cmd)
{
    for (int i = 0; i < NUM_OF_LATCHES; i++)
    {
        latches[i]->rxPkt[CMD_INDEX] = cmd;
        latches[i]->rxPkt[RX_CS_INDEX] = HxUtils::calcChecksum(latches[i]->rxPkt, SIZE_OF_RX_LATCH_PKT-1);
        for (int j = 0; j < SIZE_OF_RX_LATCH_PKT; j++)
        {
            tmuPkt[i][j] = latches[i]->rxPkt[j];
        }
    }
}


