#include "mainwindow.h"
#include "ui_mainwindow.h"

QString inputDialogSS;
QString msgBoxSS;

QString ADD;
QString ADSD;

QStringList entries;

int badKey;
QString badKeyString;

QString MainWindow::folderCheck()
{
    QStringList ADL = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    ADD = ADL.first().split(QDir::separator()).last() + "/AppData/Roaming/Kible/";
    ADSD = ADD + "AccSwitcher/";

    QString toReturn;

    QDir check(ADD);
    if (!check.exists()) {
        QMessageBox notexist;
        notexist.setWindowTitle("Does not exist");
        notexist.setText("Kible folder containing keys does not exist.\n"
                         "Do you want me to create it?");
        QPushButton* yes = notexist.addButton("Yes", QMessageBox::YesRole);
        QPushButton* no = notexist.addButton("No", QMessageBox::YesRole);
        notexist.exec();
        if (notexist.clickedButton() == yes) {
            check.mkpath(ADD); check.mkpath(ADSD); toReturn = "yes";
        } else if (notexist.clickedButton() == no) {
            toReturn = "no";
        }
    } else {
        toReturn = "yes";
        check.setPath(ADSD);
        if (!check.exists()) {check.mkpath(ADSD);}
    }
    return toReturn;
}

void MainWindow::init()
{
    QDir fileCheck(ADSD); fileCheck.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    int fileAmount = fileCheck.count();
    if (fileAmount == 0) {
        QFile getCurrKey(ADD + "info");
        getCurrKey.open(QIODevice::ReadOnly);
        QString currKey = getCurrKey.readLine();
        getCurrKey.close();
        QFile first(ADSD + "1");
        first.open(QIODevice::ReadWrite);
        first.resize(0);
        first.write(currKey.toUtf8());
        first.close();
        init();
    }
    entries = fileCheck.entryList(QDir::NoDotAndDotDot | QDir::Files);
    qDebug() << entries;
    foreach(QString entry, entries) {
        QFile getKey(ADSD + entry); getKey.open(QIODevice::ReadOnly);
        QString key = " (" + getKey.readLine().simplified() + ")"; getKey.close();
        QRadioButton* button = new QRadioButton;
        button->setText(entry + key);
        button->setStyleSheet("font-size: 14pt");
        ui->radioButtons->addWidget(button);
        button->setToolTip(entry);
    }
}

void MainWindow::clearButtons()
{
    QLayoutItem* child;
    while (ui->radioButtons->count() != 0) {
        child = ui->radioButtons->takeAt(0);

        if (child->layout() != 0) {
            delete( child->layout());
        } else if (child->widget() != 0) {
            delete child->widget();
        }

        delete child;
    }
}

QString MainWindow::getFileName()
{
    QString toReturn;
    int buttons = ui->radioButtons->count();
    QRadioButton* button;
    for (int i = 0; i < buttons; i++) {
        button = dynamic_cast<QRadioButton*>(ui->radioButtons->itemAt(i)->widget());
        if (button->isChecked()) {
            toReturn = button->toolTip();
        }
    }
    return toReturn;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if (folderCheck() == "yes") {ui->setupUi(this);} else {close();}

    inputDialogSS = "QLabel {font-size: 18px;} QPushButton {font-size: 12px;} QLineEdit {font-size: 15px;}";
    msgBoxSS = "QLabel {font-size: 18px;} QPushButton {font-size: 12px;}";

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Apply_clicked()
{
    QString fileName = getFileName();

    QFile getKey(ADSD + fileName); getKey.open(QIODevice::ReadOnly);
    QString key = getKey.readLine().simplified(); getKey.close();

    QFile setKey(ADD + "info"); setKey.open(QIODevice::ReadWrite);
    setKey.resize(0); setKey.write(key.toUtf8()); setKey.close();
}

void MainWindow::on_pushButton_Add_clicked()
{
    QInputDialog getKey;
    getKey.setWindowTitle("New account"); getKey.setLabelText("Please enter your key here:");
    getKey.setStyleSheet(inputDialogSS);
    if (badKey == 1) {getKey.setTextValue(badKeyString); badKey = 0; badKeyString = "";}
    QString key;
    if (getKey.exec() == QInputDialog::Accepted) {
        key = getKey.textValue().simplified();
        if (key.count() != 36) {
            QMessageBox badCount;
            badCount.setWindowTitle("Possibly wrong key");
            badCount.setText("Did you enter your key correctly?\n"
                             "Normally, a key is supposed to have 36 characters, the one "
                             "you entered has " + QString::number(key.count()) + " characters.\n\n"
                             "Please double check your key.");
            badCount.setStyleSheet(msgBoxSS);
            QPushButton* check = badCount.addButton("Check", QMessageBox::YesRole);
            QPushButton* ignore = badCount.addButton("Ignore", QMessageBox::YesRole);
            badCount.exec();
            if (badCount.clickedButton() == ignore) {
                QDir fileCount(ADSD);
                int count = fileCount.count() - 1;
                QFile newEntry(ADSD + QString::number(count));
                newEntry.open(QIODevice::ReadWrite);
                newEntry.resize(0);
                newEntry.write(key.toUtf8());
                newEntry.close();
                badKey = 0;
                badKeyString = "";
            } else if (badCount.clickedButton() == check) {
                badKey = 1; badKeyString = key;
                ui->pushButton_Add->click();
            }
        } else {
            QDir fileCount(ADSD);
            int count = fileCount.count() - 1;
            QFile newEntry(ADSD + QString::number(count));
            newEntry.open(QIODevice::ReadWrite);
            newEntry.resize(0);
            newEntry.write(key.toUtf8());
            newEntry.close();
            badKey = 0;
            badKeyString = "";
        }
    } else {badKey = 0;}
    clearButtons();
    init();
}

void MainWindow::on_pushButton_Edit_clicked()
{
    QString fileName = getFileName();

    if (fileName != "") {
    QFile setKey(ADSD + fileName); setKey.open(QIODevice::ReadOnly);
    QString keyString = setKey.readLine().simplified();
    setKey.close();

    QInputDialog getKey;
    getKey.setWindowTitle("Edit account"); getKey.setLabelText("Please enter your new key here:");
    getKey.setStyleSheet(inputDialogSS);
    getKey.setFixedSize(400, 10);
    if (badKey == 1) {getKey.setTextValue(badKeyString); badKey = 0; badKeyString = "";} else {getKey.setTextValue(keyString);}
    QString key;
    QString finalKey;
    if (getKey.exec() == QInputDialog::Accepted) {
        key = getKey.textValue().simplified();
        finalKey = key;
        qDebug() << key;
        if (key.count() != 36) {
            badKeyString = key;
            QMessageBox badCount;
            badCount.setWindowTitle("Possibly wrong key");
            badCount.setText("Did you enter your key correctly?\n"
                             "Normally, a key is supposed to have 36 characters, the one "
                             "you entered has " + QString::number(key.count()) + " characters.\n\n"
                             "Please double check your key.");
            badCount.setStyleSheet(msgBoxSS);
            QPushButton* check = badCount.addButton("Check", QMessageBox::YesRole);
            QPushButton* ignore = badCount.addButton("Ignore", QMessageBox::YesRole);
            badCount.exec();
            if (badCount.clickedButton() == ignore) {
                qDebug() << key;
                setKey.open(QIODevice::ReadWrite);
                setKey.resize(0);
                setKey.write(finalKey.toUtf8());
                setKey.close();
                badKey = 0;
            } else if (badCount.clickedButton() == check) {
                badKey = 1; badKeyString = key;
                ui->pushButton_Edit->click();
            }
        } else {
            setKey.open(QIODevice::ReadWrite);
            setKey.resize(0);
            setKey.write(finalKey.toUtf8());
            setKey.close();
        }
    } else {badKey = 0;}
    clearButtons();
    init();
    } else {
        QMessageBox noSelection;
        noSelection.setWindowTitle("No selection");
        noSelection.setText("You have not selected an account to edit.\n"
                            "Please select one and click Edit.");
        noSelection.setStyleSheet(msgBoxSS);
        noSelection.exec();
    }
}

void MainWindow::on_pushButton_Delete_clicked()
{
    QString fileName = getFileName();

    if (fileName != "") {
    QFile::remove(ADSD + fileName);

    QDir fileCheck(ADSD); fileCheck.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    int fileAmount = fileCheck.count();
    for (int i = 1; i < fileAmount + 1; i++) {
        int fi = i;
        QString path = ADSD + QString::number(i);
        QFile file(path);
        while (!file.exists()) {
            fi = fi + 1;
            QString path = ADSD + QString::number(fi);
            QFile check(path);
            if (check.exists()) {
                check.open(QIODevice::ReadWrite);
                QString contents = check.readAll();
                check.close();
                check.remove();
                QFile renamed(ADSD + QString::number(i));
                renamed.open(QIODevice::ReadWrite);
                renamed.resize(0);
                renamed.write(contents.toUtf8());
                break;
            }
        }
    }
    clearButtons();
    init();
    } else {
        QMessageBox noSelection;
        noSelection.setWindowTitle("No selection");
        noSelection.setText("You have not selected an account to delete.\n"
                            "Please select one and click Delete.");
        noSelection.setStyleSheet(msgBoxSS);
        noSelection.exec();
    }
}

void MainWindow::on_pushButton_Close_clicked()
{
    close();
}
