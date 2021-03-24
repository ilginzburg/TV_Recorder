#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "translations_db.h"
#include "planning_db.h"
#include "shows_db.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ExitPushButton_clicked();
    void on_UpdatePushButton_clicked();
    void on_Planning_pushButton_clicked();
    void on_Plans_Remove_button_clicked();

private:
    Ui::MainWindow *ui;
    std::ofstream logFile;
    char const* dbfile_name  = "./../data_files/myDB.db";
    char const* logfile_name = "./../data_files/log.txt";
    sqlite3* db = nullptr;
    void displTransl(QDate date, int channel);
    void displPlans();
    void displSel();
};
#endif // MAINWINDOW_H
