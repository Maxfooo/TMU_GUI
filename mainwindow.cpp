#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hexius_semicondutor_favicon_transparent.ico"));

    /****** Setup tmu object ******/
    initTMUs();
    initTMUDaemons();
    initTempCycleTMUs();
    initTMUHeatGrad();

    /****** Setup USB Communication ******/
    hxUSBComm = new HxUSBComm;
    //Connect the readback signals to an appropriate readback function
    connect(hxUSBComm, SIGNAL(commError(QString)), this, SLOT(reportError(QString)));
    connect(hxUSBComm, SIGNAL(commBusy()), this, SLOT(reportBusy()));
    //connect(hxUSBComm, SIGNAL(updateDirectReady()), this, SLOT(updateDirectGUI()));
    connect(hxUSBComm, SIGNAL(updateDataReady(USB_READBACK_TYPE)), this, SLOT(updateDataGUI(USB_READBACK_TYPE)));
    connect(hxUSBComm, SIGNAL(connectedStateChanged(bool)), this, SLOT(updateConnectedState(bool)));

    /****** LINE ENTRY VALIDATORS ******/
    hexFileValidator = new QRegExpValidator(QRegExp("^([0-9]|[a-f]){20}$"),this);
    hexValidator = new QRegExpValidator(QRegExp("^((([0-9]|[a-f]){2}[ ])|(([0-9]|[a-f]){2}))+$"),this); //only allow hex Values into validator
    hex2ByteValidator = new QRegExpValidator(QRegExp("^([0-9]|[a-f]){2}$"),this);
    hexByteValidator = new QRegExpValidator(QRegExp("^([0-9]|[a-f]){2}$"),this); //only allow hex Values into validator
    decValidator = new QRegExpValidator(QRegExp("^[0-9]+$"),this); // only allow integers
    floatValidator = new QRegExpValidator(QRegExp("^[0-9\\.]+$"),this); //only allow dec/float Values into validator



    /****** INIT FILE NAMES ******/
    mySettings.prepend(CONFIG_FILE_PREFIX);
    CONFIG_FILE_NAMES_FILE_NAME.prepend(CONFIG_FILE_PREFIX);
    TEMP_CYCLE_PROFILE_FILE_NAME.prepend(CONFIG_FILE_PREFIX);
    saveMyUI = new SaveMyUI(this, mySettings);

    /****** INIT TABS ******/
    initGeneralConfigTab();
    initTempCycleTab();
    initAdvancedTab();
    initHeatGradientTab();

    /****** INIT MISC ******/
    procRunLE = new ProcessRunningLE(ui->lineEdit_26, procRunPeriod);

    #ifdef DEBUG
        qDebug() << "Working Path without edits: " << QCoreApplication::applicationDirPath();
        qDebug() << "Application File Path: " << CONFIG_FILE_PREFIX;
        qDebug() << "Config File Name File Path: " << CONFIG_FILE_NAMES_FILE_NAME;
        qDebug() << "Temp Cycle Profile File Path: " << TEMP_CYCLE_PROFILE_FILE_NAME;
    #endif

}

MainWindow::~MainWindow()
{
    hxUSBComm->abort();
    delete hxUSBComm;
    delete ui;
}

void MainWindow::post(QString s)
{
    ui->textBrowser_2->append(s);
}

void MainWindow::initTMUs()
{
    for (auto& t: tmu)
    {
        t = new TMU();
    }
}

void MainWindow::initTMUDaemons()
{
    for (uchar i = 0; i < NUM_OF_TMUS; i++)
    {
        tmuDaemon[i] = new TMUDaemon(i, tmu[i]);
        connect(tmuDaemon[i], SIGNAL(rxTemperature(uchar)), this, SLOT(rdRegSlot(uchar)));
    }
}

void MainWindow::initTempCycleTMUs()
{
    for (uchar i = 0; i < NUM_OF_TMUS; i++)
    {
        tmuTempCycle[i] = new TMUTempCycle(i, tmu[i], this);
        connect(tmuTempCycle[i], SIGNAL(updateTMU(uchar)), this, SLOT(wrRegSlot(uchar)));
    }
}

void MainWindow::initTMUHeatGrad()
{
    tmuHeatGrad = new TmuHeatGradient();
}

void MainWindow::on_pushButton_3_clicked() // Program OTP
{
    setTMUChannel(PRIMARY_TMU_ID);
    //continue
}



//////////////////////////////////////
// USB Comm GUI Update Slots
//////////////////////////////////////
void MainWindow::updateConnectedState(bool connectedState)
{
    if(connectedState)
    {
       ui->label_26->setText("Connected");
    }
    else
    {
        ui->label_26->setText("Not Connected");
    }
}

void MainWindow::updateDirectGUI()
{
//    uchar numRxBytes;
//    QString temp;
//    hxUSBComm->getReadbackDirect(rxBuffer, &numRxBytes);
//    temp = utils::ucharArrayToQString(rxBuffer, numRxBytes);
//    ui->dataTextBrowser->setText(temp);
}

void MainWindow::updateDataGUI(USB_READBACK_TYPE readbackType)
{
    switch(readbackType)
    {
    case READBACK_DIRECT:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_DATA:
        updateDataInfo();
        break;
    case READBACK_CHIP_INFO:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_STATUS:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_LAST_PACKET:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_INPUT_BUFFER:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_DEBUG_VAR:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_FLUSH_OUTPUT:
        debugReadback();
        //updateDirectInfo();
        break;
    case READBACK_FLUSH_INPUT:
        debugReadback();
        //updateDirectInfo();
        break;
    default:
        debugReadback();
        break;
    }

}

void MainWindow::reportError(QString errorString)
{
    post(errorString);
    if (commState == COMM_DETECT_DUT_RD)
    {
        TMUDetected(false, PRIMARY_TMU_ID);
    }

    commState = COMM_IDLE;
}

void MainWindow::reportBusy()
{
    post("Comm Line is Busy");
}

void MainWindow::updateDataInfo()
{
    switch (commState)
    {
    case COMM_IDLE:

        break;

    case COMM_DETECT_DUT_RD:
        detectTMURead(readbackID);
        break;

    case COMM_TX_REG_FROM_INTERFACE:
        debugReadback();
        writeRegFromInterface(readbackID);
        break;

    case COMM_RX_REG_TO_INTERFACE:
        fillReadbackPacket(readbackID);
        readRegToInterface(readbackID);
        break;

    case COMM_TX_OTP_FROM_INTERFACE:

        break;

    case COMM_RX_OTP_TO_INTERFACE:

        break;

    case COMM_RX_MUX:

        break;

    default:

        break;
    }
}

void MainWindow::handleI2CWrite(uchar* data, uchar numOfTx, uchar numOfRx, uchar tmuPos)
{

    ////////////////////
    // HX_I2C PACKET STRUCTURE
    // <Mode>
    // <Slave Address>
    // <# of Tx Bytes>
    // <# of Rx Bytes>
    // <Byte0, Byte1, Byte2, ...>
    // <Checksum>
    ////////////////////
    /*
     * =PACKET STRUCTURE=
       MODE : [0xhh
       SLAVE_ADDR : 0xhh,
       MUX_ADDR : 0xhh,
       TMU_POS : 0xhh,
       NUM_TX : 0xhh,
       NUM_RX : 0xhh,
       {TXB0, TXB1, ... , TXBN},
       CHECKSUM : 0xhh]
     */

    uchar cmdByte = HX_PKT_I2C;
    txIndex = 0;
    //txBuffer[txIndex++] = HX_I2C_WR_RD_MODE;
    txBuffer[txIndex++] = TMU_WRITE_READ;
    txBuffer[txIndex++] = I2C_ADDRESS;
    txBuffer[txIndex++] = tmuPos; // mux address
    txBuffer[txIndex++] = I2CMuxAddr; // tmu position (Updated automatically (in heat gradient tab))
    txBuffer[txIndex++] = numOfTx;
    txBuffer[txIndex++] = numOfRx;
    for (int i = 0; i < numOfTx; i++)
    {
        txBuffer[txIndex++] = data[i];
    }

    // CS for correct packet handling in the micro
    txBuffer[txIndex++] = HxUtils::calcChecksum(txBuffer, txIndex);

#ifdef DEBUG
    qDebug() << "Tx Echo: " << HxUtils::ucharArrayToQString(txBuffer, txIndex);
    qDebug() << "Data: " << HxUtils::ucharArrayToQString(data, numOfTx);
    qDebug() << "# of TX: " << QString::number(numOfTx);
    qDebug() << "# of RX: " << QString::number(numOfRx);
#endif

    hxUSBComm->writeData(cmdByte, txBuffer, txIndex);

}

void MainWindow::writeRegFromInterface(uchar id)
{
    readbackID = id;
    setTMUChannel(id);
    tmu[id]->getWriteRegPkt(tmuTxPkt);

    commState = COMM_TX_REG_FROM_INTERFACE;

    if (tmu_index >= NUM_OF_LATCHES)
    {
        tmu_index = 0;
        commState = COMM_IDLE;
    }
    else
    {
        handleI2CWrite(tmuTxPkt[tmu_index++], SIZE_OF_TX_LATCH_PKT, 3, id);
    }
}

void MainWindow::wrRegSlot(uchar id)
{
    this->writeRegFromInterface(id);
}

void MainWindow::readRegToInterface(uchar id)
{

}

void MainWindow::rdRegSlot(uchar id)
{

}

void MainWindow::fillReadbackPacket(uchar id)
{
    //    uchar numRxBytes;
    //    uchar cmdByte;
    //    QString temp;
    //    qDebug() << "Transmission Complete";
    //    hxUSBComm->getReadbackData(&cmdByte, rxBuffer, &numRxBytes);
    //    temp = utils::ucharArrayToQString(rxBuffer, numRxBytes);
    //    ui->dataTextBrowser->setText(temp);
    uchar numRxBytes;
    uchar cmdByte;
    hxUSBComm->getReadbackData(&cmdByte, rxBuffer, &numRxBytes);
    qDebug() << utils::ucharArrayToQString(rxBuffer, numRxBytes);


}

void MainWindow::detectTMUWrite(uchar id)
{
    readbackID = id;
    setTMUChannel(id);
    commState = COMM_DETECT_DUT_RD;
#ifdef DEBUG
    uchar rxROM[5] = {CMD_RD_ROM, 0x01, 0x00, 1, 0};
    rxROM[4] = HxUtils::calcChecksum(rxROM, 4);
    handleI2CWrite(rxROM, 5, 3, id);
    // Works, returns 0x0B for address 0x0001, which is correct
#else
<<<<<<< HEAD
    commState = COMM_DETECT_DUT_RD;
    handleI2CWrite(tmu->asic_rev_sfr.rxPkt, SIZE_OF_RX_LATCH_PKT, 3, id);
=======
    handleI2CWrite(tmu[PRIMARY_TMU_ID]->asic_rev_sfr.rxPkt, SIZE_OF_RX_LATCH_PKT, 3);
>>>>>>> 5fbce2f4a39e4f3d8d8e25bd3749563883179356
#endif
}

void MainWindow::detectTMURead(uchar id)
{
    commState = COMM_IDLE;
    uchar numRxBytes;
    uchar cmdByte;
    hxUSBComm->getReadbackData(&cmdByte, rxBuffer, &numRxBytes);

#ifdef DEBUG
    qDebug() << "Detect TMU Readback: " << HxUtils::ucharArrayToQString(rxBuffer, numRxBytes);
#endif

    if (numRxBytes >= 3)
    {
        tmu[id]->setAsicRev(rxBuffer[1]);
        TMUDetected(true, id);
    }
    else
    {
        TMUDetected(false, id);
    }

}

void MainWindow::TMUDetected(bool d, uchar id)
{
    if (d)
    {
        ui->label_28->setText("Yes");
        ui->label_38->setText("0x" + QString::number(tmu[id]->getAsicRev(), 16));
    }
    else
    {
        ui->label_28->setText("No");
        ui->label_38->setText("PN");
    }
}

void MainWindow::refreshMicro()
{
    qDebug() << "Refreshing Micro";
    uchar flushInputBuff[3] = {HX_PC_FLUSH_INPUT_BUFFER, 1, 0};
    flushInputBuff[2] = HxUtils::calcChecksum(flushInputBuff, 2);
    hxUSBComm->writeDirect(flushInputBuff, 3);
    commState = COMM_IDLE;
}

void MainWindow::OTPProgrammed(bool o)
{
    if (o) { ui->label_30->setText("Yes");}
    else { ui->label_30->setText("Yes");}
}

void MainWindow::debugReadback()
{
    uchar numRxBytes;
    uchar cmdByte;
    hxUSBComm->getReadbackData(&cmdByte, rxBuffer, &numRxBytes);
    qDebug() << "DEBUG READBACK: " << utils::ucharArrayToQString(rxBuffer, numRxBytes);
}

void MainWindow::setTMUChannel(uchar id)
{
    // Use I2C to tell I2C bus mux which TMU to talk to

}


/**************************************/
//
//  GENERAL CONFIGURATION TAB
//
/**************************************/

void MainWindow::initGeneralConfigTab()
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];

    // Mux Channels
    QStringList muxChannels = tmu->getMuxChannelNames();
    for (int i = 0; i < muxChannels.size(); i++)
    {
        ui->comboBox_3->insertItem(i, muxChannels.at(i));
    }
    ui->label_12->setText(tmu->getMuxChannelUnits().at(0)); // mux output param (units)

    // Resistors
    QStringList rangeRes = tmu->getRangeNames();
    for (int i = 0; i < rangeRes.size(); i++)
    {
        ui->comboBox->insertItem(i, rangeRes.at(i));
    }

    QStringList gainRes = tmu->getGainResNames();
    for (int i = 0; i < gainRes.size(); i++)
    {
        ui->comboBox_2->insertItem(i, gainRes.at(i));
    }

    ui->lineEdit_6->setValidator(floatValidator);
    ui->lineEdit_7->setValidator(floatValidator);
    ui->lineEdit_8->setValidator(decValidator);
    ui->lineEdit_9->setValidator(floatValidator);

    ui->lineEdit->setValidator(floatValidator);
    ui->lineEdit_2->setValidator(decValidator);
    ui->lineEdit_15->setValidator(floatValidator);
    ui->lineEdit_16->setValidator(decValidator);

    ui->lineEdit_23->setValidator(floatValidator);
    ui->lineEdit_24->setValidator(floatValidator);
    ui->lineEdit_25->setValidator(floatValidator);

    ui->label_10->setText(QString("Current Limit (0mA to %1mA)").arg(CURR_LIMIT_CHIP_MAX));
    ui->label_6->setText(QString("Current Limit (0mA to %1mA)").arg(tmu->getCurrLimitEH()));

    enableIHBox();
}

void MainWindow::updateGeneralTab()
{
    if (ui->radioButton->isChecked())
    {
        enableIHBox();
    }
    else if (ui->radioButton_10->isChecked())
    {
        enableEHBox(SET_NMOS);
    }
    else if (ui->radioButton_2->isChecked())
    {
        enableEHBox(SET_NPN);
    }

    if (ui->radioButton_3->isChecked())
    {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(false);
    }
    else if (ui->radioButton_4->isChecked())
    {
        ui->lineEdit_2->setEnabled(true);
        ui->lineEdit->setEnabled(false);
    }
}

//////////////////////////////////////
// HEATER OPTIONS
//////////////////////////////////////

void MainWindow::on_radioButton_clicked() // Internal Heater RB
{
    enableIHBox();
}

void MainWindow::on_radioButton_2_clicked() // NMOS External Heater RB
{
    enableEHBox(SET_NMOS);
}

void MainWindow::on_radioButton_10_clicked() // NPN External Heater RB
{
    enableEHBox(SET_NPN);
}

void MainWindow::enableIHBox()
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    ui->groupBox_4->setEnabled(true);
    ui->groupBox_5->setEnabled(false);
    tmu->setIntOrExtHeater(SET_INTERNAL_HEATER);

    this->on_lineEdit_6_editingFinished();

    ui->lineEdit_15->setEnabled(false);
    ui->lineEdit_15->setText(QString::number(THERM_NOM_RES_REF/1000));
    ui->lineEdit_16->setEnabled(false);
    ui->lineEdit_16->setText(QString::number(THERM_NOM_BETA));
}

void MainWindow::enableEHBox(bool nmosOrNpn)
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    ui->groupBox_5->setEnabled(true);
    ui->lineEdit_7->setEnabled(true);
    ui->lineEdit_8->setEnabled(true);
    ui->lineEdit_9->setEnabled(true);
    ui->lineEdit_15->setEnabled(true);
    ui->lineEdit_16->setEnabled(true);
    ui->groupBox_4->setEnabled(false);
    tmu->setIntOrExtHeater(SET_EXTERNAL_HEATER);
    tmu->setEHTransistor(nmosOrNpn);

    this->on_lineEdit_7_editingFinished();
    this->on_lineEdit_8_editingFinished();
    this->on_lineEdit_9_editingFinished();

}

void MainWindow::on_lineEdit_6_editingFinished() // Internal Heater Current Limit
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    bool status = false;

    // Check to make sure the entered value is within the
    // respective boundaries.
    double userCurrLim = ui->lineEdit_6->text().toDouble(&status);
    if (!status)
    {
        post("The Internal Heater Current Limit could not be interpreted.");
        post("Setting the value to 0 mA.");
        userCurrLim = 0;
    }
    else if (userCurrLim > CURR_LIMIT_CHIP_MAX)
    {
        post("The Internal heater current limit is greater than the maximum.");
        post(QString("Setting the value to the maximum of %1 mA.").arg(CURR_LIMIT_CHIP_MAX));
        userCurrLim = CURR_LIMIT_CHIP_MAX;
    }
    ui->lineEdit_6->setText(QString::number(userCurrLim));
    tmu->setCurrLimit(userCurrLim);
    ui->lineEdit_10->setText(QString::number(tmu->getCurrLimitVolt()));
}

void MainWindow::on_lineEdit_9_editingFinished() // External Heater Current Limit
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userCurrLim = ui->lineEdit_9->text().toDouble(&status);
    if (!status)
    {
        post("The External Heater Current Limit could not be interpreted.");
        post("Setting the value to 0 mA.");
        userCurrLim = 0;
    }
    else if (userCurrLim > tmu->getCurrLimitEH())
    {
        post("The External heater current limit is greater than the maximum.");
        post(QString("Setting the value to the maximum of %1 mA.").arg(tmu->getCurrLimitEH()));
        userCurrLim = tmu->getCurrLimitEH();
    }
    ui->lineEdit_9->setText(QString::number(userCurrLim));
    tmu->setCurrLimit(userCurrLim);
    ui->lineEdit_14->setText(QString::number(tmu->getCurrLimitVolt()));
}

void MainWindow::on_lineEdit_7_editingFinished() // Value of heater resistors
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userEHResVal = ui->lineEdit_7->text().toDouble(&status);
    if (!status) // error
    {
        post("The External Heater Resistor value could not be interpreted.");
        post(QString("Setting the value to %1 Ohms (Internal Heater value).").arg(EXT_RES_VAL_MIN));
        userEHResVal = EXT_RES_VAL_MIN;
    }
    else if (userEHResVal < EXT_RES_VAL_MIN)
    {
        post("The External Heater Resistor value is below the minimum.");
        post(QString("Setting the value to the minimum of %1 Ohms.").arg(EXT_RES_VAL_MIN));
        userEHResVal = EXT_RES_VAL_MIN;
    }
    ui->lineEdit_7->setText(QString::number(userEHResVal));
    tmu->setEHResVal(userEHResVal);
    ui->label_6->setText(QString("Current Limit (0mA to %1mA)").arg(tmu->getCurrLimitEH()));
    // Re-call setting current limit call back because
    // it will not have been able to calculate currLimitVolt
    // without this value
    this->on_lineEdit_9_editingFinished();
}

void MainWindow::on_lineEdit_8_editingFinished() // Number of heater legs
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    unsigned int userEHResCount = ui->lineEdit_8->text().toUInt(&status);
    if (!status) // error
    {
        post("The Number of External Heater Legs could not be interpreted.");
        post("Setting the quantity to 1.");
        userEHResCount = 1;
    }
    else if (userEHResCount == 0)
    {
        post("The Number of External Heater Legs must be greater than 0.");
        post("Setting the quantity to 1.");
        userEHResCount = 1;
    }
    ui->lineEdit_8->setText(QString::number(userEHResCount));
    tmu->setEHResCount(userEHResCount);

    // Re-call setting current limit call back because
    // it will not have been able to calculate currLimitVolt
    // without this value
    this->on_lineEdit_9_editingFinished();
}

//////////////////////////////////////
// SET TEMPERATURE
//////////////////////////////////////

void MainWindow::on_radioButton_3_clicked() // Enter Celsius RB
{
    ui->lineEdit->setEnabled(true);
    ui->lineEdit_2->setEnabled(false);

}

void MainWindow::on_radioButton_4_clicked() // Enter DAC Code RB
{
    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit->setEnabled(false);
}

void MainWindow::on_lineEdit_editingFinished() // Enter Celsius LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userCelsius = ui->lineEdit->text().toDouble(&status);
    if (!status) // error
    {
        post("The Celsius value could not be interpreted.");
        post(QString("Setting the value to the minimum of %1°C.").arg(TEMP_CELSIUS_MIN));
        userCelsius = TEMP_CELSIUS_MIN;
    }
    else if (userCelsius < TEMP_CELSIUS_MIN)
    {
        post("The Celsius value must be greater than or equal to the minimum.");
        post(QString("Setting the value to the minimum of %1°C.").arg(TEMP_CELSIUS_MIN));
        userCelsius = TEMP_CELSIUS_MIN;
    }
    else if (userCelsius > TEMP_CELSIUS_MAX)
    {
        post(QString("The Celsius value must be less than or equal to the maximum "));
        post(QString("Setting value to the maximum of %1°C.").arg(TEMP_CELSIUS_MAX));
        userCelsius = TEMP_CELSIUS_MAX;
    }
    ui->lineEdit->setText(QString::number(userCelsius));
    tmu->setCelsius(userCelsius);
}

void MainWindow::on_lineEdit_2_editingFinished() // Enter DAC LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    unsigned int userCelsiusDAC = ui->lineEdit_2->text().toUInt(&status);
    if (!status) // error
    {
        post("The Celsius DAC Code value could not be interpreted.");
        post("Setting the value to 0.");
        userCelsiusDAC = 0;
    }
    else if (userCelsiusDAC > TEMP_DAC_CODE_MAX)
    {
        post(QString("The Celsius DAC Code value must be less than or equal to the maximum."));
        post(QString("Setting the value to the maximum of %1.").arg(TEMP_DAC_CODE_MAX));
        userCelsiusDAC = TEMP_DAC_CODE_MAX;
    }
    ui->lineEdit_2->setText(QString::number(userCelsiusDAC));
    tmu->setCelsius(userCelsiusDAC);
}

void MainWindow::on_lineEdit_15_editingFinished() // Thermistor Value @25C LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userThermVal = ui->lineEdit_15->text().toDouble(&status);
    if (!status) // error
    {
        post("The Thermistor Resistance value could not be interpreted.");
        post("Setting the value to 100(kOhms).");
        userThermVal = 100;
    }
    else if (userThermVal <= 0)
    {
        post("The Thermistor Resistance value must be greater than 0.");
        post("Setting the value to 100(kOhms).");
        userThermVal = 100;
    }
    ui->lineEdit_15->setText(QString::number(userThermVal));
    tmu->setThermRRef(userThermVal);
}

void MainWindow::on_lineEdit_16_editingFinished() // Thermistor Beta LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    unsigned int userThermBeta = ui->lineEdit_16->text().toUInt(&status);
    if (!status) // error
    {
        post("The Thermistor Beta value could not be interpreted.");
        post(QString("Setting the value to the nominal value of %1.").arg(THERM_NOM_BETA));
        userThermBeta = THERM_NOM_BETA;
    }
    else if (userThermBeta <= 0)
    {
        post("The Thermistor Beta value must be greater than 0.");
        post(QString("Setting the value to the nominal value of %1.").arg(THERM_NOM_BETA));
        userThermBeta = THERM_NOM_BETA;
    }
    ui->lineEdit_16->setText(QString::number(userThermBeta));
    tmu->setThermBeta(userThermBeta);
}

//////////////////////////////////////
// HEATER CONTROLLER RESISTORS
//////////////////////////////////////

void MainWindow::on_comboBox_currentIndexChanged(int index) // Range Resistor Dropdown
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // "index" param does not work
    int cbIndex = ui->comboBox->currentIndex();
    tmu->setRangeRes(cbIndex);
}

void MainWindow::on_comboBox_2_currentIndexChanged(int index) // Gain Resistor
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // "index" param does not work
    int cbIndex = ui->comboBox_2->currentIndex();
    tmu->setGainRes(cbIndex);
}

//////////////////////////////////////
// THERMAL PARAMETERS
//////////////////////////////////////

void MainWindow::on_lineEdit_23_editingFinished() // Ambient Resistance LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userResAmb = ui->lineEdit_23->text().toDouble(&status);
    if (!status)
    {
        post("The Ambient Resistance Value could not be interpreted.");
        post("Setting the value to 1 kOhm.");
        userResAmb = 1;
    }
    ui->lineEdit_23->setText(QString::number(userResAmb));
    tmu->setAmbientRes(userResAmb);
}

void MainWindow::on_lineEdit_24_editingFinished() // Material Resistance LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userResMat = ui->lineEdit_24->text().toDouble(&status);
    if (!status)
    {
        post("The Material Resistance Value could not be interpreted.");
        post("Setting the value to 5 Ohm.");
        userResMat = 5;
    }
    ui->lineEdit_24->setText(QString::number(userResMat));
    tmu->setMaterialRes(userResMat);
}

void MainWindow::on_lineEdit_25_editingFinished() // Ambient Capacitance LE
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // Check to make sure the entered value is within the
    // respective boundaries.
    bool status = false;
    double userCapAmb = ui->lineEdit_25->text().toDouble(&status);
    if (!status)
    {
        post("The Ambient Capacitance Value could not be interpreted.");
        post("Setting the value to 500 mF.");
        userCapAmb = 500;
    }
    ui->lineEdit_25->setText(QString::number(userCapAmb));
    tmu->setAmbientCap(userCapAmb);
}

//////////////////////////////////////
// PROGRAM BUTTONS
//////////////////////////////////////

void MainWindow::on_pushButton_4_clicked() // Detect TMU (Gen. Tab)
{
//    if(!_busy)
//    {
//        _busy = true;
//        retryCount = 0;
//        carbonState = CHECK_CONNECTED_STATE;
//        uchar cmdByte = HX_PKT_SPP_C_TEST;
//        uchar tempTxBuffer[3];
//        tempTxBuffer[0] = SPP_CHECK_STATE | SPP_NULL;
//        tempTxBuffer[1] = 0x00;
//        tempTxBuffer[2] = 0x00;
//        hxUSBComm->writeData(cmdByte, tempTxBuffer, 3);
//    }
    detectTMUWrite(PRIMARY_TMU_ID);
}

void MainWindow::on_pushButton_2_clicked() // Load Registers from Configuration (Gen. Tab)
{
    // the "tmu" object should already be up to date with what is
    // selected on the interface.

    writeRegFromInterface(PRIMARY_TMU_ID);
}



void MainWindow::on_pushButton_5_clicked() // Operate from Registers (Gen. Tab)
{

}

void MainWindow::on_pushButton_6_clicked() // Operate from OTP (Gen. Tab)
{

}

/**************************************/
//
//  ADVANCED TAB
//
/**************************************/

void MainWindow::initAdvancedTab()
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    QStringList latchNames = tmu->getLatchNames();

    ui->label_47->setText(tmu->latches[0]->name);
    ui->label_46->setText(tmu->latches[1]->name);
    ui->label_45->setText(tmu->latches[2]->name);
    ui->label_44->setText(tmu->latches[3]->name);
    ui->label_43->setText(tmu->latches[4]->name);

    ui->lineEdit_17->setValidator(hex2ByteValidator);
    ui->lineEdit_18->setValidator(hex2ByteValidator);
    ui->lineEdit_19->setValidator(hex2ByteValidator);
    ui->lineEdit_20->setValidator(hex2ByteValidator);
    ui->lineEdit_21->setValidator(hex2ByteValidator);

    ui->tableWidget_2->setRowCount(NUM_OF_LATCHES);
    QTableWidgetItem* latchItem;
    for (int i = 0; i < latchNames.size(); i++)
    {
        latchItem = new QTableWidgetItem();
        latchItem->setText(latchNames.at(i));
        latchItem->setFlags(latchItem->flags() ^ Qt::ItemIsEditable); // read only
        ui->tableWidget_2->setItem(i, 0, latchItem);
    }
    updateAdvancedTab();

}

void MainWindow::updateAdvancedTab()
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    QString l0 = QString("%1").arg(utils::ucharToHexQString(tmu->latches[0]->txPkt[VAL_HI_INDEX], false) + utils::ucharToHexQString(tmu->latches[0]->txPkt[VAL_LO_INDEX], false));
    QString l1 = QString("%1").arg(utils::ucharToHexQString(tmu->latches[1]->txPkt[VAL_HI_INDEX], false) + utils::ucharToHexQString(tmu->latches[1]->txPkt[VAL_LO_INDEX], false));
    QString l2 = QString("%1").arg(utils::ucharToHexQString(tmu->latches[2]->txPkt[VAL_HI_INDEX], false) + utils::ucharToHexQString(tmu->latches[2]->txPkt[VAL_LO_INDEX], false));
    QString l3 = QString("%1").arg(utils::ucharToHexQString(tmu->latches[3]->txPkt[VAL_HI_INDEX], false) + utils::ucharToHexQString(tmu->latches[3]->txPkt[VAL_LO_INDEX], false));
    QString l4 = QString("%1").arg(utils::ucharToHexQString(tmu->latches[4]->txPkt[VAL_HI_INDEX], false) + utils::ucharToHexQString(tmu->latches[4]->txPkt[VAL_LO_INDEX], false));

    ui->lineEdit_17->setText(l0);
    ui->lineEdit_18->setText(l1);
    ui->lineEdit_19->setText(l2);
    ui->lineEdit_20->setText(l3);
    ui->lineEdit_21->setText(l4);

    QStringList latchVals = tmu->getLatchValQStrLst();
    QTableWidgetItem* latchItem;
    for (int i = 0; i < latchVals.size(); i++)
    {
        latchItem = new QTableWidgetItem();
        latchItem->setText(latchVals.at(i));
        ui->tableWidget_2->setItem(i, 1, latchItem);
    }

    loadConfigFileNames();
}

void MainWindow::on_pushButton_7_clicked() // Generate (reg hex) from Interface PB
{
    this->updateAdvancedTab();
}

void MainWindow::on_pushButton_8_clicked() // Import (reg hex) from Hex File PB
{
    importHex();
}

void MainWindow::on_pushButton_9_clicked() // Export (reg) HexPB
{
    exportHex();
}

void MainWindow::exportHex()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Import HEX File"), "", tr("Hex File (*.hex);;All Files (*)"));
    if (fileName.isEmpty())
    {
        return;
    }
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //Unable to open hex file
            return;
        }
        QTextStream in(&file);
        QString outputHex = "";
        QString temp;
        QString displayString = "";

        temp = ui->lineEdit_17->text();
        for(int i = temp.size(); i < 4; i++)
        {
            temp.prepend("0");
        }
        outputHex += temp;
        displayString += temp + " ";

        temp = ui->lineEdit_18->text();
        for(int i = temp.size(); i < 4; i++)
        {
            temp.prepend("0");
        }
        outputHex += temp;
        displayString += temp + " ";

        temp = ui->lineEdit_19->text();
        for(int i = temp.size(); i < 4; i++)
        {
            temp.prepend("0");
        }
        outputHex += temp;
        displayString += temp + " ";

        temp = ui->lineEdit_20->text();
        for(int i = temp.size(); i < 4; i++)
        {
            temp.prepend("0");
        }
        outputHex += temp;
        displayString += temp + " ";

        temp = ui->lineEdit_21->text();
        for(int i = temp.size(); i < 4; i++)
        {
            temp.prepend("0");
        }
        outputHex += temp;
        displayString += temp + " ";

//        outputHex +="0000";
//        displayString += "0000";

        post("Exporting " + displayString + " to " + fileName);

        file.resize(0);
        in << outputHex;
        file.close();

    }

}

void MainWindow::importHex()
{
    QFileDialog fileDialog;
    fileDialog.setWindowTitle(tr("Import HEX File"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.resize(320,400);
    QStringList typeFilters;
    typeFilters << "Hex files (*.hex)";
    typeFilters << "Any files (*)";
    fileDialog.setNameFilters(typeFilters);
    if(fileDialog.exec() == QDialog::Accepted)
    {
        QStringList fileNameList = fileDialog.selectedFiles();
        if(fileNameList.size() != 1)
        {
            post("Import only one hex file at a time.");
            return;
        }
        QString fileName = fileNameList[0];
        if(fileName.isEmpty())
        {
            return;
        }
        else
        {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                //Unable to open hex file
                return;
            }
            QTextStream in(&file);
            QString inputHex;
            in >> inputHex;
            int pos = 0;
            if (hexFileValidator->validate(inputHex, pos) == QValidator::Acceptable)
            {
                //Load hex file into hex display
                ui->lineEdit_17->setText(inputHex.mid(0,4));
                ui->lineEdit_18->setText(inputHex.mid(4,4));
                ui->lineEdit_19->setText(inputHex.mid(8,4));
                ui->lineEdit_20->setText(inputHex.mid(12,4));
                ui->lineEdit_21->setText(inputHex.mid(16,4));
                post("Successfully import from hex file.");
                post(QString("Hex: %1").arg(inputHex));
            }
            else
            {
                //hex file is corrupted
                post("Hex File is Corrupted");
            }
        }
    }
}

void MainWindow::on_pushButton_10_clicked() // revert Configuration PB
{
    QString saveKey = ui->comboBox_4->itemText(ui->comboBox_4->currentIndex());
    saveKey = saveKey.contains(".xml") ? saveKey : saveKey + ".xml";
    saveMyUI->loadFromXML(CONFIG_FILE_PREFIX + saveKey);
    updateGeneralTab();
}

void MainWindow::on_pushButton_11_clicked() // Overwrite Configuation PB
{
    QString saveKey = ui->comboBox_4->itemText(ui->comboBox_4->currentIndex());
    saveKey = saveKey.contains(".xml") ? saveKey : saveKey + ".xml";
    saveMyUI->saveToXML(CONFIG_FILE_PREFIX + saveKey);
}

void MainWindow::on_pushButton_12_clicked() // Save As... (config) PB
{
    bool ok;
    QString saveKey = QInputDialog::getText(this, tr("Save As..."), tr("Enter Save Name:"), QLineEdit::Normal, "New Configuration", &ok);
    if(ok && !saveKey.isEmpty())
    {
        if(saveKey.size() < 41)
        {
            saveKey = saveKey.contains(".xml") ? saveKey : saveKey + ".xml";
            saveMyUI->saveToXML(CONFIG_FILE_PREFIX + saveKey);
            saveConfigFileName(saveKey);
        }
        else
        {
            post("Save name must be 40 or less characters long");
        }
    }
}

void MainWindow::saveConfigFileName(QString fname)
{
    QString fileName = CONFIG_FILE_NAMES_FILE_NAME;
    QFile file(fileName);
    QString temp = "";
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        #ifdef DEBUG
            qDebug() << "Unable to open config-file file";
        #endif
        return;
    }
    QTextStream in(&file);

    // Read everything, clean unwanted new lines
    temp = in.readAll();
    temp.replace("\n", "");
    // update new string to write to file
    temp += fname + CONFIG_FILE_NAMES_DELIMETER;
    // remove previous contents of file
    file.resize(0);
    // write new string
    in << temp;
    file.close();

    loadConfigFileNames();
}

void MainWindow::removeConfigFileName(QString fname)
{
    QString fileName = CONFIG_FILE_NAMES_FILE_NAME;
    QFile file(fileName);
    QString temp = "";
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //Unable to open hex file
        return;
    }
    QTextStream in(&file);
    temp = in.readAll();
    fname += CONFIG_FILE_NAMES_DELIMETER;
    if (temp.contains(fname))
    {
        temp.remove(fname);
        file.resize(0);
        in << temp;
        file.close();
        loadConfigFileNames();
        return;
    }
    file.close();

    loadConfigFileNames();
}

void MainWindow::loadConfigFileNames()
{
    QString fileName = CONFIG_FILE_NAMES_FILE_NAME;
    QFile file(fileName);
    QString temp = "";
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        #ifdef DEBUG
            qDebug() << "Unable to open config-file file";
        #endif
        return;
    }
    QTextStream in(&file);
    temp = in.readAll();
    qDebug() << "Load config file data: " << temp;
    ui->comboBox_4->clear();
    if (!temp.isEmpty())
    {
        QStringList configFileNames = temp.split(CONFIG_FILE_NAMES_DELIMETER);
        ui->comboBox_4->addItems(configFileNames);
    }
    file.close();

}

void MainWindow::on_pushButton_13_clicked() // Delete Configuration PB
{
    QString saveKey = ui->comboBox_4->itemText(ui->comboBox_4->currentIndex());
    removeConfigFileName(saveKey);
}

void MainWindow::on_comboBox_4_currentIndexChanged(int index) // Current Configuration CB
{

}

//////////////////////////////////////
// MULTIPLEXOR
//////////////////////////////////////


void MainWindow::on_comboBox_3_currentIndexChanged(int index) // Mux Channel Dropdown
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    // "index" param does not work
    int cbIndex = ui->comboBox_3->currentIndex();
    tmu->setMuxChannel(cbIndex);
    ui->label_12->setText(tmu->getMuxChannelUnits().at(0)); // mux output param (units)
}

void MainWindow::on_pushButton_clicked() // Mux Read Button
{

}

//////////////////////////////////////
// REFERENCE VOLTAGE
//////////////////////////////////////

void MainWindow::on_radioButton_8_clicked() // Vref Enable
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    tmu->setEnableVref(true);
}


void MainWindow::on_radioButton_9_clicked() // Vref Disable
{
    TMU* tmu = this->tmu[PRIMARY_TMU_ID];
    tmu->setEnableVref(false);
}

//////////////////////////////////////
// PROGRAM BUTTONS
//////////////////////////////////////

void MainWindow::on_pushButton_14_clicked() // Detect TMU (Adv. Tab)
{
    detectTMUWrite(PRIMARY_TMU_ID);
}

void MainWindow::on_pushButton_15_clicked() // Load Registers From Configuration (Adv. Tab)
{
    writeRegFromInterface(PRIMARY_TMU_ID);
}

void MainWindow::on_pushButton_16_clicked() // Operate from Registers (Adv. Tab)
{

}

void MainWindow::on_pushButton_17_clicked() // Operate from OTP (Adv. Tab)
{

}

/**************************************/
//
//  GRADIENT DEMO TAB
//
/**************************************/



/**************************************/
//
//  TEMPERATURE CYCLE DEMO TAB
//
/**************************************/

void MainWindow::initTempCycleTab()
{
    // init piecewise table
    ui->tableWidget->setRowCount(PIECEWISE_STEP_COUNT);
    this->on_radioButton_5_clicked();

    ui->lineEdit_3->setValidator(floatValidator);
    ui->lineEdit_4->setValidator(floatValidator);
    ui->lineEdit_5->setValidator(floatValidator);

    loadTempCycleCB();
}


void MainWindow::on_radioButton_5_clicked()
{
    ui->groupBox_9->setEnabled(true);
    ui->tableWidget->setEnabled(false);
}

void MainWindow::on_radioButton_6_clicked()
{
    ui->groupBox_9->setEnabled(false);
    ui->tableWidget->setEnabled(true);
}

//////////////////////////////////////
// SAWTOOTH TEMPERATURE CYCLE PAGE
//////////////////////////////////////


void MainWindow::on_lineEdit_3_editingFinished() // temp range start LE
{
    bool status = false;
    double tStart = ui->lineEdit_3->text().toDouble(&status);
    if (!status) // error
    {
        post("The Start Temperature value could not be interpreted.");
        post(QString("Setting the value to the minimum value of %1.").arg(TEMP_CELSIUS_MIN));
        tStart = TEMP_CELSIUS_MIN;
    }
    else if (tStart < TEMP_CELSIUS_MIN)
    {
        post("The Start Temperature value must be greater than or equal to the minimum.");
        post(QString("Setting the value to the minimum value of %1.").arg(TEMP_CELSIUS_MIN));
        tStart = TEMP_CELSIUS_MIN;
    }
    else if (tStart > TEMP_CELSIUS_MAX)
    {
        post("The Start Temperature value must be less than or equal to the maximum.");
        post(QString("Setting the value to the maximum value of %1.").arg(TEMP_CELSIUS_MAX));
        tStart = TEMP_CELSIUS_MAX;
    }
    ui->lineEdit_3->setText(QString::number(tStart));
}

void MainWindow::on_lineEdit_4_editingFinished() // temp range stop LE
{
    bool status = false;
    double tStop = ui->lineEdit_4->text().toDouble(&status);
    if (!status) // error
    {
        post("The Stop Temperature value could not be interpreted.");
        post(QString("Setting the value to the maximum value of %1.").arg(TEMP_CELSIUS_MAX));
        tStop = TEMP_CELSIUS_MAX;
    }
    else if (tStop < TEMP_CELSIUS_MIN)
    {
        post("The Stop Temperature value must be greater than or equal to the minimum.");
        post(QString("Setting the value to the minimum value of %1.").arg(TEMP_CELSIUS_MIN));
        tStop = TEMP_CELSIUS_MIN;
    }
    else if (tStop > TEMP_CELSIUS_MAX)
    {
        post("The Start Temperature value must be less than or equal to the maximum.");
        post(QString("Setting the value to the maximum value of %1.").arg(TEMP_CELSIUS_MAX));
        tStop = TEMP_CELSIUS_MAX;
    }
    ui->lineEdit_4->setText(QString::number(tStop));
}

void MainWindow::on_lineEdit_5_editingFinished() // sawtooth period LE
{
    bool status = false;
    unsigned int period = ui->lineEdit_5->text().toUInt(&status);
    if (!status) // error
    {
        post("The Period value could not be interpreted.");
        post(QString("Setting the value to the minimum value of %1.").arg(SAWTOOTH_PER_MIN));
        period = SAWTOOTH_PER_MIN;
    }
    else if (period < SAWTOOTH_PER_MIN)
    {
        post("The Start Temperature value must be greater than or equal to the minimum.");
        post(QString("Setting the value to the minimum value of %1.").arg(SAWTOOTH_PER_MIN));
        period = SAWTOOTH_PER_MIN;
    }
    ui->lineEdit_5->setText(QString::number(period));
}

void MainWindow::loadTempCycleCB()
{
    QString fileName = TEMP_CYCLE_PROFILE_FILE_NAME;
    QFile file(fileName);
    QString temp = "";
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        #ifdef DEBUG
            qDebug() << "Failed to load temp cycle Combo Box";
            qDebug() << "Unable to open temp cycle profile file";
        #endif
        return;
    }
    QTextStream in(&file);
    temp = in.readAll();
    ui->comboBox_6->clear();
    if (!temp.isEmpty())
    {
        QStringList tempCycleProfileNames = temp.split(CONFIG_FILE_NAMES_DELIMETER);
        for (auto& f: tempCycleProfileNames)
        {
            if (f.contains(".xml"))
            {
                f = f.replace(".xml", "");
            }
        }
        ui->comboBox_6->addItems(tempCycleProfileNames);
    }
    file.close();
}

void MainWindow::loadTempCycleTab(TMUTempCycle* ttc)
{
    // ttc should be set up with the correct file in it's attributes
    double tStart = TEMP_CELSIUS_MIN;
    double tStop = TEMP_CELSIUS_MAX;
    std::vector<TTData> ttdVect;
    QTableWidget* pTable = ui->tableWidget;
    if (ttc->isSawtooth())
    {
        #ifdef DEBUG
            qDebug() << "Loading sawtooth";
        #endif
        on_radioButton_5_clicked();
        ttc->getTempSpan(tStart, tStop);
        ui->lineEdit_3->setText(QString::number(tStart));
        ui->lineEdit_4->setText(QString::number(tStop));
        ui->lineEdit_5->setText(QString::number(ttc->getPeriod()));
    }
    else
    {
        #ifdef DEBUG
            qDebug() << "Loading piecewise";
        #endif
        on_radioButton_6_clicked();
        ttdVect = tmuTempCycle[PRIMARY_TMU_ID]->getPiecewise();
        for (int i = 0; i < ttdVect.size(); i++)
        {
            pTable->itemAt(0, i)->setText(QString::number(ttdVect.at(i).temp));
            pTable->itemAt(1, i)->setText(QString::number(ttdVect.at(i).time));
        }
    }
}

QString MainWindow::saveTempCycleProfileFileName(QString fname)
{
    fname = fname.contains(".xml") ? fname : fname + ".xml";

    QString fileName = TEMP_CYCLE_PROFILE_FILE_NAME;
    QFile file(fileName);
    QString temp = "";
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        #ifdef DEBUG
            qDebug() << "Unable to open temp cycle profile file";
        #endif
        return temp;
    }
    QTextStream in(&file);

    // Read everything, clean unwanted new lines
    temp = in.readAll();
    temp.replace("\n", "");
    // update new string to write to file
    temp += fname + CONFIG_FILE_NAMES_DELIMETER;
    // remove previous contents of file
    file.resize(0);
    // write new string
    in << temp;
    file.close();

    return fname;

}

void MainWindow::on_comboBox_6_currentIndexChanged(int index) // Temp Profile DropBox
{
//    QString saveKey = ui->comboBox_6->itemText(ui->comboBox_6->currentIndex()).append(".xml").prepend(CONFIG_FILE_PREFIX);

//    tmuTempCycle[PRIMARY_TMU_ID]->setFileName(saveKey);
//    tmuTempCycle[PRIMARY_TMU_ID]->loadTempProfile();
//    loadTempCycleTab(tmuTempCycle[PRIMARY_TMU_ID]);
}

void MainWindow::on_pushButton_18_clicked() // Save temp profile PB
{
    bool ok;
    QString saveKey = QInputDialog::getText(this, tr("Save Temp Profile"), tr("Enter Save Name:"), QLineEdit::Normal, "temp_profile_0", &ok);
    if(ok && !saveKey.isEmpty())
    {
        if(saveKey.size() < 41)
        {
            saveKey = saveTempCycleProfileFileName(saveKey);
            saveKey.prepend(CONFIG_FILE_PREFIX);
            qDebug() << "ONE";
            tmuTempCycle[PRIMARY_TMU_ID]->exportProfile(saveKey);
            qDebug() << "TWO";
            loadTempCycleCB();
            qDebug() << "THREE";
        }
        else
        {
            post("Save name must be 40 or less characters long");
            return;
        }
    }

    setupTMUTempCycle(tmuTempCycle[PRIMARY_TMU_ID]);
}

void MainWindow::setupTMUTempCycle(TMUTempCycle* ttc)
{
    // Setup and tmutempcycle
    bool status = false;
    double tStart = ui->lineEdit_3->text().toDouble(&status);
    double tStop = ui->lineEdit_4->text().toDouble(&status);
    unsigned int period = ui->lineEdit_5->text().toUInt(&status);
    QTableWidget* pTable = ui->tableWidget;
    QTableWidgetItem* pItem;
    TTData* ttData;
    std::vector<TTData*> ttdVect;
    if (ui->radioButton_5->isChecked())
    {
        // Sawtooth
        ttc->setSawtooth(tStart, tStop, period);
    }
    else
    {
        // Piecewise
        for (int i=0; i < PIECEWISE_STEP_COUNT; i++)
        {
            ttData = new TTData;
            pItem = pTable->itemAt(0, i);
            ttData->temp = pItem->text().toDouble(&status);
            if (!status)
            {
                post("A temperature value in the Piecewise Table is invalid.");
                return;
            }
            if (ttData->temp > TEMP_CELSIUS_MAX)
            {
                post("A temperature value in the Piecewise Table is too large.");
                post(QString("Setting value to maxmimum value of %1").arg(TEMP_CELSIUS_MAX));
                ttData->temp = TEMP_CELSIUS_MAX;
            }
            else if (ttData->temp <  TEMP_CELSIUS_MIN)
            {
                post("A temperature value in the Piecewise Table is too small.");
                post(QString("Setting value to minimum value of %1").arg(TEMP_CELSIUS_MIN));
                ttData->temp = TEMP_CELSIUS_MIN;
            }

            pItem = pTable->itemAt(1, i);
            ttData->time = pItem->text().toUInt(&status);
            if (!status)
            {
                post("A time entry in the Piecewise Table is invalid.");
                return;
            }
            if (ttData->time <  PIECEWISE_TIME_STEP_MIN)
            {
                post("A time value in the Piecewise Table is too small.");
                post(QString("Setting the value to the minimum value of %1").arg(PIECEWISE_TIME_STEP_MIN));
                ttData->time = PIECEWISE_TIME_STEP_MIN;
            }

            ttdVect.push_back(ttData);
        }
        ttc->setPiecewise(ttdVect);
    }
}

void MainWindow::on_pushButton_19_clicked() // revert to temp profile PB
{
    QString saveKey = ui->comboBox_6->itemText(ui->comboBox_6->currentIndex()).append(".xml").prepend(CONFIG_FILE_PREFIX);

    tmuTempCycle[PRIMARY_TMU_ID]->setFileName(saveKey);
    //tmuTempCycle[PRIMARY_TMU_ID]->loadTempProfile();
    tmuTempCycle[PRIMARY_TMU_ID]->importProfile();
    setupTMUTempCycle(tmuTempCycle[PRIMARY_TMU_ID]);
    //loadTempCycleTab(tmuTempCycle[PRIMARY_TMU_ID]);

    if (ui->radioButton_5->isChecked())
    {
        on_radioButton_5_clicked();
        ui->tableWidget->clearContents();
    }
    else
    {
        on_radioButton_6_clicked();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
    }
}

void MainWindow::on_pushButton_22_clicked() // Overwrite Profile PB
{

}

void MainWindow::on_pushButton_23_clicked() // Remove Profile PB
{

}

void MainWindow::on_pushButton_20_clicked() // RUN temp profile PB
{
    if (!tmuTempCycle[PRIMARY_TMU_ID]->isReady())
    {
        post("Temperature Cycle is not setup properly or ready to go.");
        return;
    }

    if (tempCycleState == TEMP_CYCLE_STOP)
    {
        // might need to check to make sure the TMUTempCycle is ready to go
        // could be function call
        // could check that a file exists...

        tempCycleState = TEMP_CYCLE_START;
        ui->pushButton_20->setText("Pause");
        procRunLE->startProcRun();
        tmuDaemon[PRIMARY_TMU_ID]->start();
        tmuTempCycle[PRIMARY_TMU_ID]->start();
    }
    else if (tempCycleState == TEMP_CYCLE_START || tempCycleState == TEMP_CYCLE_RESUME)
    {
        tempCycleState = TEMP_CYCLE_PAUSE;
        ui->pushButton_20->setText("Resume");
        tmuTempCycle[PRIMARY_TMU_ID]->pause();
        procRunLE->pauseProcRun();
    }
    else if (tempCycleState == TEMP_CYCLE_PAUSE)
    {
        tempCycleState = TEMP_CYCLE_RESUME;
        ui->pushButton_20->setText("Pause");
        tmuTempCycle[PRIMARY_TMU_ID]->resume();
        procRunLE->resumeProcRun();
    }
}

void MainWindow::on_pushButton_21_clicked() // STOP temp profile PB
{
    tempCycleState = TEMP_CYCLE_STOP;
    ui->pushButton_20->setText("Start");
    procRunLE->stopProcRun();
    tmuDaemon[PRIMARY_TMU_ID]->stop();
    tmuTempCycle[PRIMARY_TMU_ID]->stop();
}

//////////////////////////////////////
// INFO BUTTONS
//////////////////////////////////////

void MainWindow::on_infoButton_HeaterOptions_clicked()
{
    ui->textBrowser->setText(info_HeaterOptions);
}

void MainWindow::on_infoButton_InternalHeater_clicked()
{
    ui->textBrowser->setText(info_InternalHeater);
}

void MainWindow::on_infoButton_ExternalHeater_clicked()
{
    ui->textBrowser->setText(info_ExternalHeater);
}

void MainWindow::on_infoButton_SetTemperature_clicked()
{
    ui->textBrowser->setText(info_SetTemperature);
}

void MainWindow::on_infoButton_ReferenceVoltage_clicked()
{
    ui->textBrowser->setText(info_ReferenceVoltage);
}

void MainWindow::on_infoButton_LoadToRegister_clicked()
{
    ui->textBrowser->setText(info_LoadToRegisters);
}

void MainWindow::on_infoButton_ProgramOTP_clicked()
{
    ui->textBrowser->setText(info_HeaterOptions);
}

void MainWindow::on_infoButton_Multiplexor_clicked()
{
    ui->textBrowser->setText(info_Multiplexor);
}

void MainWindow::on_infoButton_HexConfigurations_clicked()
{
    ui->textBrowser->setText(info_hexConfigurations);
}

void MainWindow::on_infoButton_GenerateFromInterface_clicked()
{
    ui->textBrowser->setText(info_genFromInterface);
}

void MainWindow::on_infoButton_ImportFromHexFile_clicked()
{
    ui->textBrowser->setText(info_importHex);
}

void MainWindow::on_infoButton_ExportHexFile_clicked()
{
    ui->textBrowser->setText(info_exportHex);
}

void MainWindow::on_infoButton_ThermalParameters_clicked()
{
    ui->textBrowser->setText(info_thermalParameters);
}

//////////////////////////////////////
// TOOL BAR ACTIONS AND DEBUG TAB
//////////////////////////////////////


void MainWindow::on_actionRefresh_uC_triggered()
{
    refreshMicro();
}

<<<<<<< HEAD
//////////////////////////////////////
// HEAT GRADIENT TAB
//////////////////////////////////////

void MainWindow::initHeatGradientTab()
{
    QStringList gradProfiles = tmuHeatGrad->getGradProfNames();
    for (int i = 0; i < gradProfiles.size(); i++)
    {
        ui->comboBox_5->insertItem(i, gradProfiles.at(i));
    }


}

void MainWindow::on_spinBox_valueChanged(int arg1) // I2C Mux A2
{
    if (arg1)
    {
        I2CMuxAddr |= 0x4;
    }
    else
    {
        I2CMuxAddr &= 0xB;
    }
    updateMuxAddr();
}

void MainWindow::on_spinBox_2_valueChanged(int arg1) // I2C Mux A1
{
    if (arg1)
    {
        I2CMuxAddr |= 0x2;
    }
    else
    {
        I2CMuxAddr &= 0xD;
    }
    updateMuxAddr();
}

void MainWindow::on_spinBox_3_valueChanged(int arg1) // I2C Mux A0
{
    if (arg1)
    {
        I2CMuxAddr |= 0x1;
    }
    else
    {
        I2CMuxAddr &= 0xE;
    }
    updateMuxAddr();
}

void MainWindow::updateMuxAddr()
{
    ui->lineEdit_27->setText(QString("%1").arg(I2CMuxAddr, 7, 2, QChar('0')));
}

void MainWindow::on_comboBox_5_currentIndexChanged(int index) // Heat Gradient Profile
{

=======

void MainWindow::on_actionRead_Analog0_Latch_triggered()
{
    uchar rxAnalog0[SIZE_OF_RX_LATCH_PKT] = {CMD_READ_LATCH, ANALOG_0_NUM, 0};
    rxAnalog0[2] = HxUtils::calcChecksum(rxAnalog0, SIZE_OF_RX_LATCH_PKT-1);
    commState = COMM_IDLE;
    handleI2CWrite(rxAnalog0, SIZE_OF_RX_LATCH_PKT, 3);
}


void MainWindow::on_pushButton_24_clicked()
{
    uchar rxAnalog0[SIZE_OF_RX_LATCH_PKT] = {CMD_READ_LATCH, ANALOG_0_NUM, 0};
    rxAnalog0[2] = HxUtils::calcChecksum(rxAnalog0, SIZE_OF_RX_LATCH_PKT-1);
    commState = COMM_IDLE;
    handleI2CWrite(rxAnalog0, SIZE_OF_RX_LATCH_PKT, 3);
>>>>>>> 5fbce2f4a39e4f3d8d8e25bd3749563883179356
}
