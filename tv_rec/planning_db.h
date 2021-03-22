#ifndef PLANNING_DB_H
#define PLANNING_DB_H


#include <stdio.h>
#include <string>
#include <sqlite3.h>
#include <QDate>

int createDB (const char* dbfile_name, std::ofstream &logFile, sqlite3 **db);
int createPlanningTable(sqlite3 **db,std::ofstream &logFile);
int addPlanning (QString name, int channel, QDate date, QTime startRecTime, QTime startShowTime,
             QTime endRecTime, QTime endShowTime, int rating,
                   sqlite3 **db,std::ofstream &logFile);
int erasePlanning (QDate date, int channel,  QTime startRecTime, QTime endRecTime,
                   sqlite3 **sqldb,std::ofstream &logFile);
int updatePlanning (QDate date, int channel, QString name, QTime startRecTime, QTime endRecTime,
                    sqlite3 **sqldb,std::ofstream &logFile);


int printPlanning (sqlite3 **db, std::string &data,std::ofstream &logFile);
int callbackPlanning(void *param, int argc, char **argv, char **azColName);

bool isExistsPlanning(QString name, int channel, QDate date, QTime startShowTime, sqlite3 **sqldb,std::ofstream &logFile);

char findMax (int channel, QDate date, QTime startRecTime, QTime endRecTime, std::string &max_rating,
              sqlite3 **db, std::ofstream &logFile);
int callbackfindMax(void *param, int argc, char **argv, char **azColName);

#endif // PLANNING_DB_H
