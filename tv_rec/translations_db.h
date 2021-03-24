#ifndef TRANSLATIONS_DB_H
#define TRANSLATIONS_DB_H

#include <fstream>
#include <QLocale>
#include "planning_db.h"
#include "shows_db.h"


struct Translation
{
    QDate date;
    QTime start_time;
    QTime end_time;
    QString channel;
    QString name;
    QString rating;
    bool isPlanned;
};


struct Translations
{
    Translation** data;
    int           size;
};


void displayTranslations(QDate date, int channel, QString &s);
QString readLine();
void readTranslation (Translation* translation,sqlite3 **sqldb, std::ofstream &logFile);
void fillTranslations(sqlite3 **sqldb, std::ofstream &logFile);
Translation* createTranslation(sqlite3 **sqldb, std::ofstream &logFile);
void eraseTranslations();
void deleteTranslation(Translation* t);
int findTranslation(QDate date, QTime startRecTime, QTime endRecTime, int channel);
void addNewPlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel,
                    sqlite3 **sqldb,std::ofstream &logFile);
void RemovePlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel,
                    sqlite3 **sqldb,std::ofstream &logFile);
void addNewShows(QDate date, QTime startRecTime, QTime endRecTime, int channel, int idx,
                 sqlite3 **sqldb,std::ofstream &logFile);
#endif // TRANSLATIONS_DB_H
