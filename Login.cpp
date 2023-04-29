/*********************************************************************
 * @file   Login.cpp
 * @brief  用于 Qt 窗口登录、注册用户相关
 *         这里会访问数据库以查找定位用户
 *
 * @version 1.0
 * @author  陆畅
 * @date    2022.06.04
 *********************************************************************/
#include "Login.h"
#include "ui_Login.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCloseEvent>

#include "Register.h"
#include "DataBase.h"
/**
 * @brief 设置窗口，连接数据库并准备登录
 * @param parent  用于 Qt 界面编程
 */
Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // 窗口大小固定

    if (DataBase::instance()->IsOpen())
    {
        ui->label_status->setText("成功连接至数据库。");
        QTextStream(stdout) << "Connect OK!";
    }
    else
    {
        ui->label_status->setText("数据库连接失败！！！");
        QTextStream(stdout) << "Connect Fail!";
    }
}
/**
 * @brief 析构函数
 */
Login::~Login()
{
    delete ui;
}
/**
 * @brief 清除ui页面输入框的用户名与密码
 */
void Login::Clear()
{
    ui->edit_password->clear();
    ui->edit_username->clear();
}

/**
 * @brief 用于 Qt 程序关闭时触发的事件
 * @param event 接受 Qt 程序关闭事件的详细内容
 */
void Login::closeEvent (QCloseEvent *event)
{
    //弹窗询问用户是否真的要退出，二次确认
    QMessageBox::StandardButton mboxSelect = QMessageBox::question( this, "退出", "确定退出吗",
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

    if (mboxSelect != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();// 程序退出
    }
}
/**
 * @brief 用于 Qt 界面登录按钮的触发事件
 */
void Login::on_button_login_clicked()
{
    //执行trim，以忽略首尾可能的空格
    QString username = ui->edit_username->text().trimmed();
    QString password = ui->edit_password->text().trimmed();

    if (username.isEmpty())
    {
        ui->label_status->setText("用户名不得为空!");
        return;
    }
    else if (password.isEmpty())
    {
        ui->label_status->setText("密码不得为空!");
        return;
    }
    //初始化database查询
    QSqlQuery query = QSqlQuery(DataBase::instance()->GetDatabase());

    query.prepare(
        "SELECT login_in (:username, :password)"
    );
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec())
    {
        if (query.first())
        {
            if (query.value(0).toInt() > 0)
            {
                ui->label_status->setText("登录成功");
                DataBase::instance()->ChangePriority(query.value(0).toInt());
                this->hide();
                emit this->LoginIn(username);
                return;
            }
            else
            {
                ui->label_status->setText("用户名或密码错误！！");
                return;
            }
        }
        else
        {
            QMessageBox::warning(this, "注意", "查找失败!");
            qDebug() << query.lastError();
        }
    }
    else
    {
        QMessageBox::warning(this, "注意", "查找失败!");
        qDebug() << query.lastError();
        return;
    }
}
/**
 * @brief 用于 Qt 界面注册按钮的触发事件
 */
void Login::on_button_register_clicked()
{
    Register *r = new Register(this);
    r->setModal(true);
    r->show();
}
