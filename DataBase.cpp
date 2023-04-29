/*********************************************************************
 * @file   DataBase.cpp
 * @brief  数据库连接，创建了三个不同权限的连接
 *
 * @version 1.1
 * @author 胡航语
 * @date   2022.06.04
 *
 * @oldversion 1.0
 * @oldauthor 胡航语
 * @date   2022.05.28
 *********************************************************************/
#include "DataBase.h"
#include <QSqlError>
/**
  * @brief 构造函数，默认权限是1
  * @param QObject Qt的对象的父类
  */
DataBase::DataBase(QObject* parent): QObject(parent)
{
    priority = 1;
}
/**
  * @brief 析构函数，关闭所有连接
  */
DataBase::~DataBase()
{
    m_db_admin.close();
    m_db_senior.close();
    m_db_admin.close();
}
/**
  * @brief 当前连接的数据库
  */
DataBase* DataBase::instance()  // 所有只使用一个静态对象
{
    static DataBase db_controller;
    return &db_controller;
}
/**
  * @brief 建立三个权限不同的数据库连接
  */
void DataBase::SetupDB()
{
    m_db_admin = QSqlDatabase::addDatabase("QODBC");
    m_db_admin.setHostName("localhost");
    m_db_admin.setPort(3306);
    m_db_admin.setDatabaseName("normal");
    m_db_admin.setUserName("normal");
    m_db_admin.setPassword("normal");

    m_db_senior = QSqlDatabase::addDatabase("QODBC", "senior");
    m_db_senior.setHostName("localhost");
    m_db_senior.setPort(3306);
    m_db_senior.setDatabaseName("senior");
    m_db_senior.setUserName("senior");
    m_db_senior.setPassword("senior");

    m_db_admin = QSqlDatabase::addDatabase("QODBC", "admin");
    m_db_admin.setHostName("localhost");
    m_db_admin.setPort(3306);
    m_db_admin.setDatabaseName("admin");
    m_db_admin.setUserName("admin");
    m_db_admin.setPassword("admin");

//    query_normal = QSqlQuery(m_db_admin);
//    query_senior = QSqlQuery(m_db_senior);
//    query_admin = QSqlQuery(m_db_admin);
}
/**
  * @brief 判断数据库是否打开
  * @return true 已打开；false 未打开
  */
bool DataBase::IsOpen()
{
    if (priority == 1)
        return m_db_admin.isOpen();
    else if (priority == 2)
        return m_db_senior.isOpen();
    else if (priority == 3)
        return m_db_admin.isOpen();
    return m_db_admin.isOpen();
}
/**
  * @brief 判断数据库是否链接
  * @return true 成功连接；false 连接断开
  */
bool DataBase::Connect()
{
    if (priority == 1)
        return m_db_admin.open();
    else if (priority == 2)
        return m_db_senior.open();
    else if (priority == 3)
        return m_db_admin.open();
    return m_db_admin.open();
}
/**
  * @brief 关闭链接
  */
void DataBase::Disconnect()
{
    if (priority == 1)
        m_db_admin.close();
    else if (priority == 2)
        m_db_senior.close();
    else if (priority == 3)
        m_db_admin.close();
}
/**
  * @brief 修改权限priority
  * @param 修改的权限等级
  */
void DataBase::ChangePriority(int i)
{
    Disconnect();
    priority = i;
    Connect();
}
/**
  * @brief 获取权限priority
  * @return 权限等级
  */
int DataBase::GetPriority()
{
    return priority;
}
/**
  * @brief 查看错误
  * @return 数据库的最后一次错误
  */
QString DataBase::GetError()
{
    if (priority == 1)
        return m_db_admin.lastError().text();
    else if (priority == 2)
        return m_db_senior.lastError().text();
    else if (priority == 3)
        return m_db_admin.lastError().text();
    return m_db_admin.lastError().text();
}
/**
  * @brief 获取现在使用的访问对象
  * @return 使用的访问对象
  */
//QSqlQuery& DataBase::Query()
//{
//    if (priority == 1)
//        return query_normal;
//    else if (priority == 2)
//        return query_senior;
//    else if (priority == 3)
//        return query_admin;
//    return query_normal;
//}
/**
  * @brief 获取现在使用的数据库对象
  * @return 使用的数据库
  */
QSqlDatabase& DataBase::GetDatabase()
{
    if (priority == 1)
        return m_db_admin;
    else if (priority == 2)
        return m_db_senior;
    else if (priority == 3)
        return m_db_admin;
    return m_db_admin;
}
