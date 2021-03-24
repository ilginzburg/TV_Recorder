#include "shows_db.h"
#include <string>
#include <fstream>

int createShowsTable(sqlite3 **db,std::ofstream &logFile)
{
    char *err_msg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS Shows"
                      "(Date TEXT, StartShowTime TEXT, Channel INT,"
                      "Name TEXT, EndShowTime TEXT, Rating INT,"
                      "UNIQUE(Date, StartShowTime,Channel,Name,EndShowTime,Rating));";
    int rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
    if (rc == SQLITE_OK )
    {
        logFile<<"Таблица \"Shows\" создана"<<'\n';
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


int addShow (QString name, int channel, QDate date, QTime startShowTime,
             QTime endShowTime, int rating, sqlite3 **db,std::ofstream &logFile)
{
    char *err_msg = 0;
    QString insertQStr = "INSERT OR IGNORE INTO Shows VALUES('"
                          + date.toString("yyyy-MM-dd")      + "','"
                          + startShowTime.toString("hh:mm")  + "',"
                          + QString::number(channel)         + " ,'"
                          + name                             + "','"
                          + endShowTime.toString("hh:mm")    + "',"
                          + QString::number(rating)          + ")";

    std::string insertStr = insertQStr.toStdString();
    int  rc = sqlite3_exec(*db, insertStr.c_str(), 0, 0, &err_msg);
    if (rc == SQLITE_OK )
    {
         logFile<<"Передача: "<<name.toStdString()<<"добавлена в список передач"<<'\n';
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



int eraseShow (QString name, int channel, QDate date, QTime startShowTime,
               sqlite3 **db,std::ofstream &logFile)
{
    char *err_msg = 0;
    QString insertQStr = "DELETE FROM Shows WHERE Name = '"
                                                   +  name                           +
                         "' AND Date = '"          + date.toString("yyyy-MM-dd")     +
                         "' AND StartShowTime = '" + startShowTime.toString("hh:mm") +
                         "' AND Channel = "        + QString::number(channel);
    int  rc = sqlite3_exec(*db, insertQStr.toStdString().c_str(), 0, 0, &err_msg);
    if (rc == SQLITE_OK )
    {
        logFile<<"\nПередача: "<<name.toStdString()<<" удалена из списка передач Shows"<<'\n';
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


int eraseShows (int channel, QDate date, QTime startRecTime, QTime endRecTime,
                sqlite3 **db,std::ofstream &logFile)
{
    char *err_msg = 0;
    QString insertQStr = "DELETE FROM Shows WHERE  Date = '"
                                                        + date.toString("yyyy-MM-dd")   +
                           "' AND ((StartShowTime <= '" + endRecTime.toString("hh:mm")  +
                           "' AND StartShowTime   >= '" + startRecTime.toString("hh:mm")+
                           "') OR (EndShowTime    <= '" + endRecTime.toString("hh:mm")  +
                           "' AND EndShowTime     >= '" + startRecTime.toString("hh:mm")+
                           "')) AND Channel        =  " + QString::number(channel);
    int  rc = sqlite3_exec(*db, insertQStr.toStdString().c_str(), 0, 0, &err_msg);
    if (rc == SQLITE_OK )
    {
         logFile<<"\nПередачи с: "<<startRecTime.toString("hh:mm").toStdString()
                <<" до "<<endRecTime.toString("hh:mm").toStdString()
                <<" удалены из списка передач Shows"<<'\n';
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


int printShows (sqlite3 **db, std::string &data,std::ofstream &logFile)
{
    char *err_msg = 0;
    const char* sql = "SELECT * FROM Shows" ;
    int rc = sqlite3_exec(*db, sql, &callbackShows, &data, &err_msg);
    if (rc != SQLITE_OK )
    {
        logFile<<"Не удалось найти данные"<<'\n';
        logFile<<"SQL error:  "<<err_msg<<'\n';
        sqlite3_free(err_msg);
        sqlite3_close(*db);
        return 1;
     }
    return 0;
}



int callbackShows(void *param, int argc, char **argv, char **azColName)
{
    QDate date;
    QTime time;
    QLocale curLocale(QLocale("ru_RU"));
    QLocale::setDefault(curLocale);
    std::string *data = static_cast<std::string*>(param);
    for (int i = 0; i < argc; i++)
    {
        if(i == 0)
        {
           date = QDate::fromString(argv[i],"yyyy-MM-dd");
           *data += QLocale().toString(date,QLocale::ShortFormat).toStdString()+ "  ";
        }
        else if((i==1) || (i==4))
        {
            time = QTime::fromString(argv[i],"hh:mm");
            *data += "\t"+  QLocale().toString(time,QLocale::ShortFormat).toStdString()+ " ";
        }
        else if(i==3)
        {
            std::string sname = std::string(argv[i] ? argv[i] : "NULL");
            QString qsname = QString ::fromStdString(sname);
            sname =  qsname.leftJustified(35,' ',false).toStdString();
            *data += "\t" + sname;
        }
        else
           *data +=  "\t" + std::string(argv[i] ? argv[i] : "NULL");
    }
    *data += '\n';
    return 0;
}
