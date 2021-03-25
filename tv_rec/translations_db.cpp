#include "translations_db.h"
#include <QDebug>
#include <QMessageBox>

void displayTranslations(QDate date, int channel, QString &s, Translations &transldb)
{
    QLocale curLocale(QLocale("ru_RU"));
    QLocale::setDefault(curLocale);
    s += "ДАТА\t\tВРЕМЯ\t\tКАНАЛ\tПЕРЕДАЧА \t        РЕЙТИНГ   ПЛАНИР\n";
    s += "-------------------------------------------------------------------------------------------------------------------------------\n";
    for(int i = 0; i < transldb.size; ++i )
    {
        if(channel == transldb.data[i]->channel.toInt() && date.operator==(transldb.data[i]->date))
        {
            s += QLocale().toString(transldb.data[i]->date,QLocale::ShortFormat)       +"\t"+
                 QLocale().toString(transldb.data[i]->start_time,QLocale::ShortFormat) +"-"+
                 QLocale().toString(transldb.data[i]->end_time,QLocale::ShortFormat)   +"\t"+
                 transldb.data[i]->channel                                             +"\t"+
                 transldb.data[i]->name.leftJustified(30,' ',false)                    +"\t"+
                 transldb.data[i]->rating                                              +"\t"+
                (transldb.data[i]->isPlanned ? "да":"нет")                             +"\n";
        }
    }
}


void addNewPlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel,
                    sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile)
{
    int idx = findTranslation(date, startRecTime, endRecTime, channel, transldb);
    if(idx < 0)
    {
        logFile<<"Во временном интервале с "<<startRecTime.toString().toStdString()<<" до "
               <<endRecTime.toString().toStdString() <<" передачу найти не удалось"<<std::endl;
        return;
    }
    if(transldb.data[idx]->isPlanned == true)
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
             updatePlanning(date, channel, transldb.data[idx]->name, startRecTime, endRecTime, sqldb, logFile);
             msgBox.setStandardButtons(QMessageBox::Close );
             msgBox.setInformativeText(" ");
             msgBox.setText("Планирование изменено.");
             msgBox.exec();
             addNewShows(date, startRecTime, endRecTime, channel, idx, sqldb, transldb, logFile);
               return;
          case QMessageBox::No:
              return;
          default:
              break;
        }

    }
    transldb.data[idx]->isPlanned = true;
    addNewShows(date, startRecTime, endRecTime, channel, idx, sqldb, transldb, logFile);
    addPlanning (transldb.data[idx]->name, channel, date, startRecTime, transldb.data[idx]->start_time,
                 endRecTime, transldb.data[idx]->end_time, transldb.data[idx]->rating.toInt(), sqldb, logFile);
    logFile<<"Передача "<< transldb.data[idx]->name.toStdString()
           <<" внесена в список планирований"<<std::endl;
}


void addNewShows(QDate date, QTime startRecTime, QTime endRecTime, int channel, int idx,
                 sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile)
{
    for(int i = 0; i < transldb.size; ++i )
    {
        if(channel == transldb.data[i]->channel.toInt()
           && date.operator==(transldb.data[i]->date)
           && startRecTime.operator<=(transldb.data[i]->end_time)
           && endRecTime.operator>=(transldb.data[i]->start_time))
        {
            addShow (transldb.data[i]->name, transldb.data[i]->channel.toInt(),
                     transldb.data[i]->date, transldb.data[i]->start_time,
                     transldb.data[i]->end_time, transldb.data[i]->rating.toInt(), sqldb, logFile);
            logFile<<"Передача "<< transldb.data[idx]->name.toStdString() <<" внесена в список передач\n";
        }
    }
}



int findTranslation(QDate date, QTime startRecTime, QTime endRecTime, int channel, Translations &transldb)
{
    for(int i = 0; i < transldb.size; ++i )
    {
         if(channel == transldb.data[i]->channel.toInt()
            && date.operator==(transldb.data[i]->date)
            && startRecTime.operator<=(transldb.data[i]->start_time)
            && endRecTime.operator>=(transldb.data[i]->end_time))
            return i;
    }
    return -1;
}


void fillTranslations(sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile)
{
     std::ifstream inFile;
     char const* infile_name  = "./../data_files/translations.ini";
     inFile.open(infile_name);
     transldb.size = 0;
     transldb.data = nullptr;
     while(!inFile.eof())
     {
         Translation** new_data = new Translation*[transldb.size + 1];
         for(int i = 0; i < transldb.size; ++i)
             new_data[i] = transldb.data[i];
         if (transldb.data != nullptr)
             delete[] transldb.data;
         transldb.data = new_data;
         transldb.data[transldb.size] = createTranslation(sqldb, logFile, inFile);
         logFile<<"Найдена трансляция: "<<transldb.data[transldb.size]->name.toStdString()<<"\n";
         ++transldb.size;
     }
}


Translation* createTranslation(sqlite3 **sqldb, std::ofstream &logFile ,std::ifstream &inFile)
{
    Translation* t = new Translation;
    readTranslation (t, sqldb, logFile,inFile);
    return t;
}


void readTranslation(Translation* translation,sqlite3 **sqldb, std::ofstream &logFile ,std::ifstream &inFile)
{
    translation->date        = QDate::fromString(readLine(inFile),"dd MM yyyy");
    translation->start_time  = QTime::fromString(readLine(inFile),"hh:mm");
    translation->end_time    = QTime::fromString(readLine(inFile),"hh:mm");
    translation->channel     = readLine(inFile);
    translation->name        = readLine(inFile);
    translation->rating      = readLine(inFile);
    if(isExistsPlanning(translation->name, translation->channel.toInt(), translation->date, translation->start_time, sqldb, logFile))
    {
        translation->isPlanned = true;
        logFile<<"Найдена запланированная передача: "<<translation->name.toStdString()<<"\n";
        return;
    }
    translation->isPlanned = false;
}


QString readLine(std::ifstream &inFile)
{
    std::string l;
    std::getline(inFile, l);
    return QString::fromStdString(l);
}


void eraseTranslations(Translations &transldb)
{
    for(int i = 0; i < transldb.size;)
         deleteTranslation(transldb.data[i]);
}


void deleteTranslation(Translation* t)
{
    if (t == nullptr)
        return;
    delete t;
}


void RemovePlanning(QDate date, QTime startRecTime, QTime endRecTime, int channel,
                    sqlite3 **sqldb, Translations &transldb, std::ofstream &logFile)
{
    int idx = findTranslation(date, startRecTime, endRecTime, channel, transldb);
    if(idx < 0)
    {
        logFile<<"Во временном интервале с "<<startRecTime.toString().toStdString()<<" до "
               <<endRecTime.toString().toStdString() <<" передача не обнаружена"<<std::endl;
        return;
    }
    if(transldb.data[idx]->isPlanned)
    {
        logFile<<"Передача "<< transldb.data[idx]->name.toStdString()
               <<" обнаружена и будет удалена из запланированных"<<std::endl;
        transldb.data[idx]->isPlanned = false;
    }
    erasePlanning (date, channel, startRecTime, endRecTime, sqldb, logFile);
    logFile<<"Передача "<< transldb.data[idx]->name.toStdString()
          <<" удалена из списка планирований"<<std::endl;
    eraseShows (channel, date, startRecTime, endRecTime, sqldb, logFile);
}

