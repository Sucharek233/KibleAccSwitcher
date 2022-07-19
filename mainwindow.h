#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString folderCheck();
    void init();

    void clearButtons();

    QString getFileName();

private slots:
    void on_pushButton_Apply_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_Edit_clicked();

    void on_pushButton_Delete_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
