#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

/**
 * @brief 数据库连接类
 */
class DataBase : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase m_db_normal;  // normal连接
    QSqlDatabase m_db_senior;  // senior连接
    QSqlDatabase m_db_admin;  // admin连接
    int priority;  // 权限  1 normal 2 senior 3 admin
    QSqlQuery query_normal;  // normal访问对象
    QSqlQuery query_senior;  // senior访问对象
    QSqlQuery query_admin;  // admin访问对象


public:
    DataBase(QObject *parent = 0);
    ~DataBase();

    void SetupDB();

    QSqlQuery &Query();

    bool IsOpen();

    bool Connect();

    void Disconnect();

    QString GetError();

    void ChangePriority(int i);

    int GetPriority();

    QSqlDatabase &GetDatabase();

    static DataBase *instance();
};

#endif
