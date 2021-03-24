#ifndef SHOWS_DB_H
#define SHOWS_DB_H
#include <stdio.h>
#include <string>
#include <sqlite3.h>
#include <QDate>
#include <QLocale>

int createShowsTable(sqlite3 **db,std::ofstream &logFile);
int addShow(QString name, int channel, QDate date, QTime startShowTime,
            QTime endShowTime, int rating, sqlite3 **db,std::ofstream &logFile);
int eraseShow(QString name, int channel, QDate date, QTime startShowTime,
              sqlite3 **db,std::ofstream &logFile);
int eraseShows(int channel, QDate date, QTime startRecTime, QTime endRecTime,
               sqlite3 **db,std::ofstream &logFile);
int printShows(sqlite3 **db, std::string &data,std::ofstream &logFile);
int callbackShows(void *param, int argc, char **argv, char **azColName);


#endif // SHOWS_DB_H
