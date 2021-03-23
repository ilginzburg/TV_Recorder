#include "translations_db.h"
#include <QDebug>
#include <QMessageBox>

std::ifstream inFile;
Translations db;

void displayTranslations(QDate date, int channel, QString &s)
{
    QLocale curLocale(QLocale("ru_RU"));
    QLocale::setDefault(curLocale);
    s += "ДАТА\t\tВРЕМЯ\t\tКАНАЛ\tПЕРЕДАЧА \t        РЕЙТИНГ   ПЛАНИР\n";
    s += "-------------------------------------------------------------------------------------------------------------------------------\n";
    for(int i = 0; i < db.size; ++i )
    {
        if(channel == db.data[i]->channel.toInt() && date.operator==(db.data[i]->date))
        {
            s += QLocale().toString(db.data[i]->date,QLocale::ShortFormat)       +"\t"+
                 QLocale().toString(db.data[i]->start_time,QLocale::ShortFormat) +"-"+
                 QLocale().toString(db.data[i]->end_time,QLocale::ShortFormat)   +"\t"+
                 db.data[i]->channel                                             +"\t"+
                 db.data[i]->name.leftJustified(30,' ',false)                    +"\t"+
                 db.data[i]->rating                                              +"\t"+
                (db.data[i]->isPlanned ? "да":"нет")                             +"\n";
        }
    }
}


void addNewPlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel, sqlite3 **sqldb,std::ofstream &logFile)
{
    int idx = findTranslation(date, startRecTime, endRecTime, channel);
    if(idx < 0)
    {
        logFile<<"Во временном интервале с "<<startRecTime.toString().toStdString()<<" до "
               <<endRecTime.toString().toStdString() <<" передачу найти не удалось \n";
        return;
    }
    if(db.data[idx]->isPlanned == true)
    {

        QMessageBox msgBox;
        msgBox.setText("Эта передача была запланирована раннее.");
        msgBox.setInformativeText("Изменить планирование?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret)
        {
          case QMessageBox::Yes:
             updatePlanning(date, channel, db.data[idx]->name, startRecTime, endRecTime, sqldb, logFile);
             msgBox.setStandardButtons(QMessageBox::Close );
             msgBox.setInformativeText(" ");
             msgBox.setText("Планирование изменено.");
             msgBox.exec();
             addNewShows(date, startRecTime, endRecTime, channel, idx, sqldb, logFile);
               return;
          case QMessageBox::No:
              return;
          default:
              break;
        }

    }
    db.data[idx]->isPlanned = true;
    addNewShows(date, startRecTime, endRecTime, channel, idx, sqldb, logFile);
    addPlanning (db.data[idx]->name, channel, date, startRecTime, db.data[idx]->start_time,
                 endRecTime, db.data[idx]->end_time, db.data[idx]->rating.toInt(), sqldb, logFile);
    logFile<<"Передача "<< db.data[idx]->name.toStdString() <<" внесена в список планирований\n";
}


void addNewShows(QDate date, QTime startRecTime, QTime endRecTime, int channel, int idx, sqlite3 **sqldb,std::ofstream &logFile)
{
    for(int i = 0; i < db.size; ++i )
    {
        if(channel == db.data[i]->channel.toInt() && date.operator==(db.data[i]->date)
        && startRecTime.operator<=(db.data[i]->end_time)&& endRecTime.operator>=(db.data[i]->start_time))
        {
            addShow (db.data[i]->name, db.data[i]->channel.toInt(), db.data[i]->date, db.data[i]->start_time,
                    db.data[i]->end_time, db.data[i]->rating.toInt(), sqldb, logFile);
            logFile<<"Передача "<< db.data[idx]->name.toStdString() <<" внесена в список передач\n";
        }
    }
}



int findTranslation(QDate date, QTime startRecTime, QTime endRecTime, int channel)
{
    for(int i = 0; i < db.size; ++i )
    {
         if(channel == db.data[i]->channel.toInt() && date.operator==(db.data[i]->date)
           && startRecTime.operator<=(db.data[i]->start_time)&& endRecTime.operator>=(db.data[i]->end_time))
            return i;
    }
    return -1;
}




void fillTranslations(sqlite3 **sqldb, std::ofstream &logFile)
{
     char const* infile_name  = "./../data_files/translations.ini";
     inFile.open(infile_name);
     db.size = 0;
     db.data = nullptr;
     while(!inFile.eof())
     {
         Translation** new_data = new Translation*[db.size + 1];
         for(int i = 0; i < db.size; ++i)
             new_data[i] = db.data[i];
         if (db.data != nullptr)
             delete[] db.data;
         db.data = new_data;
         db.data[db.size] = createTranslation(sqldb, logFile);
         logFile<<"Найдена трансляция: "<<db.data[db.size]->name.toStdString()<<"\n";
         ++db.size;
     }
}


Translation* createTranslation(sqlite3 **sqldb, std::ofstream &logFile)
{
    Translation* t = new Translation;
    readTranslation (t, sqldb, logFile);
    return t;
}



void readTranslation(Translation* translation,sqlite3 **sqldb, std::ofstream &logFile)
{
    translation->date        = QDate::fromString(readLine(),"dd MM yyyy");
    translation->start_time  = QTime::fromString(readLine(),"hh:mm");
    translation->end_time    = QTime::fromString(readLine(),"hh:mm");
    translation->channel     = readLine();
    translation->name        = readLine();
    translation->rating      = readLine();
    if(isExistsPlanning(translation->name, translation->channel.toInt(), translation->date, translation->start_time, sqldb, logFile))
    {
        translation->isPlanned = true;
        logFile<<"Найдена запланированная передача: "<<translation->name.toStdString()<<"\n";
        return;
    }
    translation->isPlanned = false;
}





QString readLine ()
{
    std::string l;
    std::getline(inFile, l);
    return QString::fromStdString(l);
}



void eraseTranslations()
{
    for(int i = 0; i < db.size;)
         deleteTranslation(db.data[i]);

}


void deleteTranslation(Translation* t)
{
    if (t == nullptr)
        return;
    delete t;
}



void RemovePlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel, sqlite3 **sqldb,std::ofstream &logFile)
{
    int idx = findTranslation(date, startRecTime, endRecTime, channel);
    if(idx < 0)
    {
        logFile<<"Во временном интервале с "<<startRecTime.toString().toStdString()<<" до "
               <<endRecTime.toString().toStdString() <<" передача не обнаружена \n";
        return;
    }
    if(db.data[idx]->isPlanned)
    {
        logFile<<"Передача "<< db.data[idx]->name.toStdString() <<" обнаружена и будет удалена из запланированных\n";
        db.data[idx]->isPlanned = false;
    }

    erasePlanning (date, channel, startRecTime, endRecTime, sqldb, logFile);
    logFile<<"Передача "<< db.data[idx]->name.toStdString() <<" удалена из списка планирований\n";

    eraseShows (channel, date, startRecTime, endRecTime, sqldb, logFile);

}

