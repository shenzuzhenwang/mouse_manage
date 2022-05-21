#include "DataBase.h"
#include <QSqlError>

DataBase::DataBase(QObject* parent): QObject(parent)
{
    priority = 1;
}

DataBase::~DataBase()
{
    m_db1.close();
    m_db2.close();
    m_db3.close();
}

DataBase* DataBase::instance()  // 所有只使用一个静态对象
{
    static DataBase db_controller;
    return &db_controller;
}

void DataBase::SetupDB()
{
    m_db1 = QSqlDatabase::addDatabase("QODBC");
    m_db1.setHostName("localhost");
    m_db1.setPort(3306);
    m_db1.setDatabaseName("normal");
    m_db1.setUserName("normal");
    m_db1.setPassword("normal");

    m_db2 = QSqlDatabase::addDatabase("QODBC", "senior");
    m_db2.setHostName("localhost");
    m_db2.setPort(3306);
    m_db2.setDatabaseName("senior");
    m_db2.setUserName("senior");
    m_db2.setPassword("senior");

    m_db3 = QSqlDatabase::addDatabase("QODBC", "admin");
    m_db3.setHostName("localhost");
    m_db3.setPort(3306);
    m_db3.setDatabaseName("admin");
    m_db3.setUserName("admin");
    m_db3.setPassword("admin");

    query1 = QSqlQuery(m_db1);
    query2 = QSqlQuery(m_db2);
    query3 = QSqlQuery(m_db3);
}

bool DataBase::IsOpen()
{
    if (priority == 1)
        return m_db1.isOpen();
    else if (priority == 2)
        return m_db2.isOpen();
    else if (priority == 3)
        return m_db3.isOpen();
    return m_db1.isOpen();
}

bool DataBase::Connect()
{
    if (priority == 1)
        return m_db1.open();
    else if (priority == 2)
        return m_db2.open();
    else if (priority == 3)
        return m_db3.open();
    return m_db1.open();
}

void DataBase::Disconnect()
{
    if (priority == 1)
        m_db1.close();
    else if (priority == 2)
        m_db2.close();
    else if (priority == 3)
        m_db3.close();
}

void DataBase::ChangePriority(int i)
{
    Disconnect();
    priority = i;
    Connect();
}

int DataBase::GetPriority()
{
    return priority;
}

QString DataBase::GetError()
{
    if (priority == 1)
        return m_db1.lastError().text();
    else if (priority == 2)
        return m_db2.lastError().text();
    else if (priority == 3)
        return m_db3.lastError().text();
    return m_db1.lastError().text();
}

QSqlQuery& DataBase::Query()
{
    if (priority == 1)
        return query1;
    else if (priority == 2)
        return query2;
    else if (priority == 3)
        return query3;
    return query1;
}

QSqlDatabase& DataBase::GetDatabase()
{
    if (priority == 1)
        return m_db1;
    else if (priority == 2)
        return m_db2;
    else if (priority == 3)
        return m_db3;
    return m_db1;
}
