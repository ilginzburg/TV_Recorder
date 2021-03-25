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


void displayTranslations(QDate date, int channel, QString &s, Translations &transldb);
QString readLine(std::ifstream &inFile);
void readTranslation (Translation* translation,sqlite3 **sqldb, std::ofstream &logFile, std::ifstream &inFile);
void fillTranslations(sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile);
Translation* createTranslation(sqlite3 **sqldb, std::ofstream &logFile, std::ifstream &inFile);
void eraseTranslations(Translations &transldb);
void deleteTranslation(Translation* t);
int findTranslation(QDate date, QTime startRecTime, QTime endRecTime, int channel, Translations &transldb);
void addNewPlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel,
                    sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile);
void RemovePlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel,
                    sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile);
void addNewShows(QDate date, QTime startRecTime, QTime endRecTime, int channel, int idx,
                 sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile);
#endif // TRANSLATIONS_DB_H
