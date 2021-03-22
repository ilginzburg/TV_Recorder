#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    logFile.open(logfile_name);
    createDB(dbfile_name, logFile, &db);
    createShowsTable(&db, logFile);
    createPlanningTable(&db, logFile);
    ui->setupUi(this);
    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    ui->channel_spinBox->setRange(1,2);
    ui->TV_Progs_dateEdit->setMinimumDate(QDate::currentDate());
    ui->TV_Progs_dateEdit->setMaximumDate(QDate::currentDate().addDays(14));
    fillTranslations(&db,logFile);

    displPlans();
    displSel();
    displTransl(ui->TV_Progs_dateEdit->date(),ui->channel_spinBox->value());


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::displTransl(QDate date, int channel)
{
    QString s;
    displayTranslations(date, channel, s);
    ui->tvShows_textBrowser->setText(s);
}


void MainWindow::displPlans()
{
    std::string str;
    printPlanning (&db, str, logFile);
    ui->Planning_textBrowser->setText(QString::fromStdString(str));
}

void MainWindow::displSel()
{
    std::string str;
    printShows (&db, str, logFile);
    ui->Selected_textBrowser->setText(QString::fromStdString(str));
}


void MainWindow::on_ExitPushButton_clicked()
{
    eraseTranslations();
    logFile.close();
    close();
}


void MainWindow::on_UpdatePushButton_clicked()
{
    displTransl(ui->TV_Progs_dateEdit->date(),ui->channel_spinBox->value());

}

void MainWindow::on_Planning_pushButton_clicked()
{

    addNewPlanning(ui->TV_Progs_dateEdit->date(),
                   ui->PlanningStartTimeEdit->time(),
                   ui->PlanningEndTimeEdit->time(),
                   ui->channel_spinBox->value(),
                   &db,
                   logFile);
    displPlans();
    displSel();

}

void MainWindow::on_Plans_Remove_button_clicked()
{
    RemovePlanning(ui->Plans_dateEdit->date(),
                   ui->Plans_PlanningStartTimeEdit->time(),
                   ui->Plans_PlanningEndTimeEdit->time(),
                   ui->Plans_channel_spinBox->value(),
                   &db,
                   logFile);
    displPlans();
    displSel();
}

