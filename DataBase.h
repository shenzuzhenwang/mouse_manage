#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class DataBase : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase m_db1;
    QSqlDatabase m_db2;
    QSqlDatabase m_db3;
    int priority;
    QSqlQuery query1;
    QSqlQuery query2;
    QSqlQuery query3;

public:
    DataBase(QObject* parent = 0);
    ~DataBase();

    void SetupDB();

    QSqlQuery& Query();

    bool IsOpen();

    bool Connect();

    void Disconnect();

    QString GetError();

    void ChangePriority(int i);

    int GetPriority();

    QSqlDatabase& GetDatabase();

    static DataBase* instance();
};

#endif
