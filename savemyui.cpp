#include "savemyui.h"

SaveMyUI::SaveMyUI(QMainWindow *parent)
{
    this->parent = parent;
    #ifdef SAVE_SETTINGS
        mySettings = new QSettings(ORGANIZATION_NAME, APPLICATION_NAME, parent);
    #endif
}

SaveMyUI::SaveMyUI(QMainWindow *parent, QString xmlFName)
{
    this->parent = parent;
    xmlFileName = xmlFName;
    #ifdef SAVE_SETTINGS
        mySettings = new QSettings(ORGANIZATION_NAME, APPLICATION_NAME, parent);
    #endif
}

void SaveMyUI::setFileName()
{
    xmlFileName = QFileDialog::getOpenFileName(parent, "Open Image", QDir::currentPath(), "XML Files (*.xml);;All Files (*.*)");
    if (xmlFileName.length() == 0)
    {
        xmlFileName = XML_FILE_NAME;
    }
    else if (!xmlFileName.contains(".xml"))
    {
        xmlFileName.append(".xml");
    }
}

void SaveMyUI::saveToXML()
{
    if (xmlFileName.length() == 0)
    {
        setFileName();
    }
    saveToXML(xmlFileName);
}

void SaveMyUI::saveToXML(const QString& fileName)
{
    QString errStr = "Could not open XML file for writing.";
    QString val;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        msgBox.setText(errStr);
        msgBox.exec();
        return;
    }

    QDomDocument doc;
    QDomProcessingInstruction header = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(header);

    QDomElement root = doc.createElement(XML_ROOT);
    doc.appendChild(root);

    QDomElement subroot;
    QDomElement key;
    QDomText val1;
    QDomElement tiKey; // table item key (for the QTableWidget)
    QDomText tiText; // table item text



    /***************************/
    // QRadioButtons
    /***************************/
    subroot = doc.createElement(Q_RAD_BTN);
    root.appendChild(subroot);
    for (QRadioButton* r: parent->centralWidget()->findChildren<QRadioButton*>())
    {
        // Check if it is checked or not
        val = r->isChecked() == true ? "1" : "0";
        key = doc.createElement(r->objectName());
        subroot.appendChild(key);

        val1 = doc.createTextNode(val);
        key.appendChild(val1);
    }

    /***************************/
    // QRadioButtons
    /***************************/
    subroot = doc.createElement(Q_CHK_BOX);
    root.appendChild(subroot);
    for (QCheckBox* c: parent->centralWidget()->findChildren<QCheckBox*>())
    {
        val = c->isChecked() == true ? "1" : "0";
        key = doc.createElement(c->objectName());
        subroot.appendChild(key);

        val1 = doc.createTextNode(val);
        key.appendChild(val1);
    }

    /***************************/
    // QLineEdit
    /***************************/
    subroot = doc.createElement(Q_LINE_EDIT);
    root.appendChild(subroot);
    // Checking for all QLineEdits
    for (QLineEdit* le: parent->centralWidget()->findChildren<QLineEdit*>())
    {
        key = doc.createElement(le->objectName());
        subroot.appendChild(key);

        // Copy and save text from the QLineEdit
        val1 = doc.createTextNode(le->text());
        key.appendChild(val1);
    }

    /***************************/
    // QLabel
    /***************************/
    subroot = doc.createElement(Q_LABEL);
    root.appendChild(subroot);
    for (QLabel* l: parent->centralWidget()->findChildren<QLabel*>())
    {
        key = doc.createElement(l->objectName());
        subroot.appendChild(key);

        val1 = doc.createTextNode(l->text());
        key.appendChild(val1);
    }

    /***************************/
    // QTableWidget
    /***************************/
    subroot = doc.createElement(Q_TABLE);
    root.appendChild(subroot);
    int cCnt=0, rCnt=0;
    for (QTableWidget* t: parent->centralWidget()->findChildren<QTableWidget*>())
    {
        // Need to store the items in the table as well as the table
        key = doc.createElement(t->objectName());
        subroot.appendChild(key);

        // need to append tiKey to key as a child now
        cCnt = t->columnCount();
        rCnt = t->rowCount();
        //qDebug() << "Row Count: " << QString::number(rCnt);
        //qDebug() << "Column Count: " << QString::number(cCnt);
        for (int i = 0; i < cCnt; i++)
        {
            for (int j = 0; j < rCnt; j++)
            {
                if (t->item(j, i) == 0)
                {
                    continue;
                }

                tiKey = doc.createElement(QString("r%1_c%2").arg(j).arg(i));
                key.appendChild(tiKey);
                //qDebug() << "Point: " << QString::number(j) + "," + QString::number(i);
                tiText = doc.createTextNode(t->item(j, i)->text());
                tiKey.appendChild(tiText);
            }
        }
    }

    /***************************/
    // QComboBox
    /***************************/
    subroot = doc.createElement(Q_CMBO_BOX);
    root.appendChild(subroot);
    for (QComboBox* c: parent->centralWidget()->findChildren<QComboBox*>())
    {
        val = QString::number(c->currentIndex());
        key = doc.createElement(c->objectName());
        subroot.appendChild(key);

        val1 = doc.createTextNode(val);
        key.appendChild(val1);
    }

    QTextStream output(&file);
    output << doc.toString();
    file.close();
}

void SaveMyUI::loadFromXML()
{
    if (xmlFileName.length() == 0)
    {
        setFileName();
    }
    qDebug() << "Loading file: " << xmlFileName;
    loadFromXML(xmlFileName);
}

void SaveMyUI::loadFromXML(const QString& fileName)
{
    QString errStr = "Could not open XML file to READ/LOAD.";
    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
    {
        msgBox.setText(errStr);
        msgBox.exec();
        //qDebug() << errStr;
        return;
    }

    // grab all the xml nodes under the tag Q_RAD_BTN

    /***************************/
    // QRadioButton
    /***************************/
    QDomNodeList keyList = doc.elementsByTagName(Q_RAD_BTN).at(0).childNodes();
    QDomNode key;
    for (int i = 0; i < keyList.count(); i++)
    {
        key = keyList.at(i);
        // Find the corresponding child in the mainwindow (centralWidget) using the key
        // which is the object name (how convenient!) then set the value to what's listed
        // in mySettings
        parent->centralWidget()->findChild<QRadioButton*>(key.nodeName())->setChecked(!!key.toElement().text().toInt());

    }

    /***************************/
    // QCheckBox
    /***************************/
    keyList = doc.elementsByTagName(Q_CHK_BOX).at(0).childNodes();
    for (int i = 0; i < keyList.count(); i++)
    {
        key = keyList.at(i);
        parent->centralWidget()->findChild<QCheckBox*>(key.nodeName())->setChecked(!!key.toElement().text().toInt());
    }

    /***************************/
    // QLineEdit
    /***************************/
    keyList = doc.elementsByTagName(Q_LINE_EDIT).at(0).childNodes();
    for (int i = 0; i < keyList.count(); i++)
    {
        key = keyList.at(i);
        parent->centralWidget()->findChild<QLineEdit*>(key.nodeName())->setText(key.toElement().text());
    }

    /***************************/
    // QLabel
    /***************************/
    keyList = doc.elementsByTagName(Q_LABEL).at(0).childNodes();
    for (int i = 0; i < keyList.count(); i++)
    {
        key = keyList.at(i);
        parent->centralWidget()->findChild<QLabel*>(key.nodeName())->setText(key.toElement().text());
    }

    /***************************/
    // QTableWidget
    /***************************/
    keyList = doc.elementsByTagName(Q_TABLE).at(0).childNodes();
    QTableWidget* tempTW;
    QTableWidgetItem* tempTWI;
    QStringList rowColTemp;
    QDomNodeList tiKey;
    int row=0,col=0;
    for (int i = 0; i < keyList.count(); i++)
    {
        key = keyList.at(i); // "tableWidget"
        tiKey = key.childNodes(); // List of "rY_cX"
        tempTW = parent->centralWidget()->findChild<QTableWidget*>(key.nodeName()); // QTableWidget*
        for (int i = 0; i < tiKey.count(); i++)
        {
            rowColTemp = tiKey.at(i).nodeName().split("_");
            row = rowColTemp[0].remove('r').toInt();
            col = rowColTemp[1].remove('c').toInt();
            tempTWI = new QTableWidgetItem(tiKey.at(i).toElement().text());
            tempTW->setItem(row, col, tempTWI);
        }

    }

    /***************************/
    // QComboBox
    /***************************/
    keyList = doc.elementsByTagName(Q_CMBO_BOX).at(0).childNodes();
    bool status = false;
    for (int i = 0; i < keyList.count(); i++)
    {
        key = keyList.at(i);
        parent->centralWidget()->findChild<QComboBox*>(key.nodeName())->setCurrentIndex(key.toElement().text().toInt(&status));
    }

    file.close();
}


#ifdef SAVE_SETTINGS

QSettings::Status SaveMyUI::writeSettings()
{
    // ------------------------------------------
    // PLACE IN THE DESTRUCTOR OF MAINWINDOW
    // ------------------------------------------


    // OPTIONAL ERROR CATCH
//    if (saveMyUI->writeSettings() == QSettings::NoError)
//    {
//        qDebug() << "Successfully SAVED UI state.";
//    }
//    else
//    {
//        qDebug() << "Something went wrong while trying to SAVE the UI state.";
//    }
//    delete ui;


    QString val;

    // Create groups for each of the widgets we are interested in.
    mySettings->beginGroup(Q_RAD_BTN);

    // Find and loop through all of the respective objects storing the
    // pertinent information to mySettings under the objects name as
    // the key
    for (QRadioButton* r: parent->centralWidget()->findChildren<QRadioButton*>())
    {
        // I do this simple conversion just because it makes it easier to convert
        // back to boolean
        val = r->isChecked() == true ? "1" : "0";
        mySettings->setValue(r->objectName(), val);
    }
    mySettings->endGroup();

    mySettings->beginGroup(Q_CHK_BOX);
    for (QCheckBox* c: parent->centralWidget()->findChildren<QCheckBox*>())
    {
        val = c->isChecked() == true ? "1" : "0";
        mySettings->setValue(c->objectName(), val);
    }
    mySettings->endGroup();

    mySettings->beginGroup(Q_LINE_EDIT);
    for (QLineEdit* le: parent->centralWidget()->findChildren<QLineEdit*>())
    {
        mySettings->setValue(le->objectName(), le->text());
    }
    mySettings->endGroup();

    mySettings->beginGroup(Q_LABEL);
    for (QLabel* l: parent->centralWidget()->findChildren<QLabel*>())
    {
        mySettings->setValue(l->objectName(), l->text());
    }
    mySettings->endGroup();

    // Check for errors
    mySettings->sync();
    return mySettings->status();
}

QSettings::Status SaveMyUI::readSettings()
{
    // --------------------------------------
    // PLACE IN THE CONSTRUCTOR OF MAINWINDOW
    // --------------------------------------


    // OPTIONAL ERROR CATCH
//    if (saveMyUI->readSettings() == QSettings::NoError)
//    {
//        qDebug() << "Successfully LOADED UI state.";
//    }
//    else
//    {
//        qDebug() << "Something went wrong while trying to LOAD the UI state.";
//    }


    // Loop through all of the groups in mySettings
    for (QString& cg : mySettings->childGroups())
    {
        // Need to individually check for each object type as we will need to use the object
        // type in a template later.
        if (cg == Q_RAD_BTN)
        {
            // begin the group (so that we can look into it)
            mySettings->beginGroup(cg);

            // Loop through all of the child keys which represent each of the object names
            for (QString& k : mySettings->childKeys())
            {
                // Find the corresponding child in the mainwindow (centralWidget) using the key
                // which is the object name (how convenient!) then set the value to what's listed
                // in mySettings
                parent->centralWidget()->findChild<QRadioButton*>(k)->setChecked(mySettings->value(k).toBool());
            }
            mySettings->endGroup();
        }
        else if (cg == Q_CHK_BOX)
        {
            mySettings->beginGroup(cg);
            for (QString& k : mySettings->childKeys())
            {
                parent->centralWidget()->findChild<QCheckBox*>(k)->setChecked(mySettings->value(k).toBool());
            }
            mySettings->endGroup();
        }
        else if (cg == Q_LINE_EDIT)
        {
            mySettings->beginGroup(cg);
            for (QString& k : mySettings->childKeys())
            {
                parent->centralWidget()->findChild<QLineEdit*>(k)->setText(mySettings->value(k).toString());
            }
            mySettings->endGroup();
        }
        else if (cg == Q_LABEL)
        {
            mySettings->beginGroup(cg);
            for (const auto& k : mySettings->childKeys())
            {
                parent->centralWidget()->findChild<QLabel*>(k)->setText(mySettings->value(k).toString());
            }
            mySettings->endGroup();
        }
    }

    return mySettings->status();
}

// Use only if you have already setup and saved to the
// internal 'settings' file (mySettings)
void SaveMyUI::printMySettings()
{
    for (const auto& cg : mySettings->childGroups())
    {
        qDebug() << cg;
        mySettings->beginGroup(cg);
        for (const auto& ck : mySettings->childKeys())
        {
            qDebug() << ck << mySettings->value(ck);
        }
        mySettings->endGroup();
    }
}
#endif
