#include "planning_db.h"
#include <QDebug>
#include <fstream>

int createDB (const char* dbfile_name, std::ofstream &logFile, sqlite3 **db)
{
    int rc = sqlite3_open(dbfile_name, db);
    if(rc != SQLITE_OK)
    {
        logFile<<"Can't open database:"<<sqlite3_errmsg(*db)<<'\n';
        sqlite3_close(*db);
        return(1);
    }
    else
        logFile<<"The database SQLite version: "<<sqlite3_libversion()<<" has been opened successfully"<<'\n';
    return 0;
}


int createPlanningTable(sqlite3 **db,std::ofstream &logFile)
{
    char *err_msg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS Планирования"
                      "(Дата TEXT, ВремяНачалаЗаписи TEXT, "
                      "ВремяНачалаПередачи TEXT, Канал INT, Название TEXT, "
                      "ВремяОкончанияПередачи TEXT,ВремяОкончанияЗаписи TEXT, "
                      "Рейтинг INT, МаксРейтинг INT);";
        int rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
        if (rc == SQLITE_OK ) {
            logFile<<"Таблица \"Планирования\" создана"<<'\n';
        }
        else
        {
            logFile<<"SQL error: "<<err_msg<<'\n';
            sqlite3_free(err_msg);
            sqlite3_close(*db);
            return 1;
        }
   return 0;
}


int addPlanning (QString name, int channel, QDate date, QTime startRecTime, QTime startShowTime,
             QTime endRecTime, QTime endShowTime, int rating, sqlite3 **db,std::ofstream &logFile)
{

    std::string max_rating;
    findMax(channel, date, startRecTime, endRecTime, max_rating, db, logFile);
    char *err_msg = 0;
    QString insertQStr = "INSERT INTO Планирования VALUES('"
                          + date.toString("yyyy-MM-dd")          + "','"
                          + startRecTime.toString("hh:mm")       + "','"
                          + startShowTime.toString("hh:mm")      + "', "
                          + QString::number(channel)             + " ,'"
                          + name                                 + "','"
                          + endShowTime.toString("hh:mm")        + "','"
                          + endRecTime.toString("hh:mm")         + "', "
                          + QString::number(rating)              + " , "
                          + QString::fromStdString( max_rating)  + ")  ";
    std::string insertStr = insertQStr.toStdString();
    int  rc = sqlite3_exec(*db, insertStr.c_str(), 0, 0, &err_msg);
        if (rc == SQLITE_OK )
        {
             logFile<<"Передача: "<<name.toStdString()<<"добавлена"<<'\n';
        }
        else
        {
            logFile<<"SQL error:  "<<err_msg<<'\n';
            sqlite3_free(err_msg);
            sqlite3_close(*db);
            return 1;
        }
    return 0;
}


char findMax (int channel, QDate date, QTime startRecTime, QTime endRecTime, std::string &max_rating, sqlite3 **db, std::ofstream &logFile)
{
    if(db == nullptr)
        return -1;
    char *err_msg = 0;
    QString insertQStr =    "SELECT MAX(Rating) FROM Shows WHERE Date = '"
                                                            + date.toString("yyyy-MM-dd")   +
                               "' AND ((StartShowTime <= '" + endRecTime.toString("hh:mm")  +
                                  "' AND StartShowTime >='" + startRecTime.toString("hh:mm")+
                                  "') OR (EndShowTime <= '" + endRecTime.toString("hh:mm")  +
                                    "' AND EndShowTime >='" + startRecTime.toString("hh:mm")+
                                       "')) AND Channel = " + QString::number(channel);
    std::string insertStr = insertQStr.toStdString();
    logFile<<insertStr.c_str();
    int rc = sqlite3_exec(*db, insertStr.c_str(), &callbackfindMax, &max_rating, &err_msg);
    if (rc != SQLITE_OK )
    {
        logFile<<"Не удалось найти данные"<<'\n';
        logFile<<"SQL error:  "<<err_msg<<'\n';
        sqlite3_free(err_msg);
        sqlite3_close(*db);
        return -1;
     }
  return 0;
}


int callbackfindMax(void *param, int argc, char **argv, char **azColName)
{
     std::string *data = static_cast<std::string *>(param);
     for (int i = 0; i < argc; i++)
          *data = std::string(argv[i] ? argv[i] : "NULL");
    return 0;
}





int erasePlanning (QDate date, int channel,  QTime startRecTime, QTime endRecTime, sqlite3 **sqldb,std::ofstream &logFile)
{
    char *err_msg = 0;
    QString insertQStr = "DELETE FROM Планирования WHERE Дата = '"
                           +  date.toString("yyyy-MM-dd") +
                           "' AND ВремяНачалаЗаписи =      '"+ startRecTime.toString("hh:mm")  +
                           "' AND ВремяОкончанияЗаписи   = '"+ endRecTime.toString("hh:mm")    +
                           "' AND Канал =                   "+ QString::number(channel) ;
    std::string insertStr = insertQStr.toStdString();
    int  rc = sqlite3_exec(*sqldb, insertStr.c_str(), 0, 0, &err_msg);
        if (rc == SQLITE_OK )
        {
            logFile<<"\nПланирование: "<<startRecTime.toString().toStdString()<<"удалено"<<'\n';
        }
        else
        {
            logFile<<"SQL error:  "<<err_msg<<'\n';
            sqlite3_free(err_msg);
            sqlite3_close(*sqldb);
            return 1;
        }
        return 0;
}




int updatePlanning (QDate date, int channel, QString name, QTime startRecTime, QTime endRecTime, sqlite3 **sqldb,std::ofstream &logFile)
{
    char *err_msg = 0;
    QString insertQStr = "UPDATE Планирования "
                         "SET ВремяНачалаЗаписи = '" +startRecTime.toString("hh:mm") +
                         "',ВремяОкончанияЗаписи = '" +endRecTime.toString("hh:mm") +
                           "' WHERE Дата = '"+  date.toString("yyyy-MM-dd") +
                           "' AND Название =      '"+ name  +
                           "' AND Канал =                   "+ QString::number(channel) ;

    std::string insertStr = insertQStr.toStdString();
    logFile<<insertStr.c_str();
    int  rc = sqlite3_exec(*sqldb, insertStr.c_str(), 0, 0, &err_msg);
        if (rc == SQLITE_OK )
        {
            logFile<<"\nПланирование: "<<name.toStdString()<<"обновлено"<<'\n';
        }
        else
        {
            logFile<<"SQL error:  "<<err_msg<<'\n';
            sqlite3_free(err_msg);
            sqlite3_close(*sqldb);
            return 1;
        }
        return 0;
}














bool isExistsPlanning(QString name, int channel, QDate date, QTime startShowTime, sqlite3 **sqldb,std::ofstream &logFile)
{
    char *err_msg = 0;
    std::string  data = " ";
    QString insertQStr =   "SELECT * FROM Планирования WHERE Название = '"
                           +  name +
                           "' AND Канал = "+ QString::number(channel) +
                           "  AND Дата = '"+ date.toString("yyyy-MM-dd") +
                           "' AND ВремяНачалаПередачи = '"+ startShowTime.toString("hh:mm")+"'";
    std::string insertStr = insertQStr.toStdString();
    int  rc = sqlite3_exec(*sqldb, insertStr.c_str(), &callbackPlanning, &data, &err_msg);
        if (rc == SQLITE_OK )
        {
            logFile<<"The search in Planning for: "<<name.toStdString()<<"has been performed successfully"<<'\n';
        }
        else
        {
            logFile<<"SQL search error:  "<<err_msg<<'\n';
            sqlite3_free(err_msg);
            sqlite3_close(*sqldb);
        }
        if(data == " ")
            return false;
        return true;
}


int printPlanning (sqlite3 **db, std::string &data,std::ofstream &logFile)
{
    char *err_msg = 0;
    char* sql = (char*)"SELECT * FROM Планирования" ;
    int rc = sqlite3_exec(*db, sql, &callbackPlanning, &data, &err_msg);

        if (rc != SQLITE_OK )
        {
           logFile<<"Failed to select data"<<'\n';
           logFile<<"SQL error:  "<<err_msg<<'\n';
           sqlite3_free(err_msg);
           sqlite3_close(*db);
           return 1;
         }
    return 0;
}

int callbackPlanning(void *param, int argc, char **argv, char **azColName)
{
    QDate date;
    QTime time;
    QLocale curLocale(QLocale("ru_RU"));
    QLocale::setDefault(curLocale);

    std::string *data = static_cast<std::string *>(param);

    for (int i = 0; i < argc; i++)
    {
        if(i == 0)
        {
           date = QDate::fromString(argv[i],"yyyy-MM-dd");
           *data += QLocale().toString(date,QLocale::ShortFormat).toStdString()+ "  ";
        }
        else if((i==1) || (i==2) || (i==5) || (i==6))
        {
            time = QTime::fromString(argv[i],"hh:mm");
            *data += "  "+ QLocale().toString(time,QLocale::ShortFormat).toStdString()+ "  ";
        }
        else if(i==4)
        {
            std::string sname = std::string(argv[i] ? argv[i] : "NULL");
            QString qsname = QString ::fromStdString(sname);
            sname =  qsname.leftJustified(35,' ',false).toStdString();
            *data +=  "\t\t" + sname;
        }
        else if (i == 7)
        {
            *data +=  "\t  ";
            continue;
        }
        else
           *data +=  " " + std::string(argv[i] ? argv[i] : "NULL");
    }
    *data += '\n';
     return 0;
}
