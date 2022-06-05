#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

/**
 * @brief ���ݿ�������
 */
class DataBase : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase m_db_normal;  // normal����
    QSqlDatabase m_db_senior;  // senior����
    QSqlDatabase m_db_admin;  // admin����
    int priority;  // Ȩ��  1 normal 2 senior 3 admin
    QSqlQuery query_normal;  // normal���ʶ���
    QSqlQuery query_senior;  // senior���ʶ���
    QSqlQuery query_admin;  // admin���ʶ���

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
