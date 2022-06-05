/*********************************************************************
 * @file   Regeister.c
 * @brief  给用户提供注册功能
 * 
 * @version 1.1
 * @author 左安民
 * @date   2022.06.06
 * @old_version 1.0
 * @old_author 左安民
 * @date   2022.06.04
 *********************************************************************/

#include "Register.h"
#include "ui_Register.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QRegExp>

/**
 * @brief 打开窗口
 * @param parent 传递值为0
 */
Register::Register(QWidget *parent) : QDialog(parent), ui(new Ui::Register)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // 窗口大小固定
}

/**
 * @brief 退出窗口
 */
Register::~Register()
{
    delete ui;
}

/**
 * @brief 检查窗口内输入是否合法
 * @return false：注册数据不合法    true：注册数据合法
 */
bool Register::CheckInputs()
{
    
    QRegExp regexMail("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");//邮箱格式
    regexMail.setCaseSensitivity(Qt::CaseInsensitive); //大小写不敏感

    QRegExp regexPhone("^[0-9]{8,11}$");//电话格式
    regexPhone.setCaseSensitivity(Qt::CaseInsensitive);//大小写不敏感

    //名字不为空
    if (ui->edit_name->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "名字不能为空");
        return false;
    }//密码不为空
    else if (ui->edit_password->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "密码不能为空");
        return false;
    }//重复密码不为空
    else if (ui->edit_password2->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "重复密码不能为空");
        return false;
    }//两次密码输入一致
    else if (QString::compare(ui->edit_password->text().trimmed(), ui->edit_password2->text().trimmed()) != 0)
    {
        QMessageBox::warning(this, "注意", "重复密码与密码不符");
        return false;
    }//邮箱不为空
    else if (ui->edit_mail->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "邮箱不能为空");
        return false;
    }//检查邮箱输入格式
    else if (!regexMail.exactMatch(ui->edit_mail->text().trimmed()))
    {
        QMessageBox::warning(this, "注意", "邮箱格式不符");
        return false;
    }//电话不为空
    else if (ui->edit_phone->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "电话不能为空");
        return false;
    }//检查电话格式
    else if (!regexPhone.exactMatch(ui->edit_phone->text().trimmed()))
    {
        QMessageBox::warning(this, "注意", "电话号码长度应为8-11");
        return false;
    }//同意协议
    else if (!ui->check_service->checkState())
    {
        QMessageBox::warning(this, "注意", "需要同意协议!");
        return false;
    }

    return true;
}

/**
 * @brief 页面输入清空
 */
void Register::Clear()
{
    ui->edit_name->clear();
    ui->edit_password->clear();
    ui->edit_password2->clear();
    ui->edit_mail->clear();
    ui->edit_phone->clear();
    ui->edit_address->clear();
}

/**
 * @brief 进行注册检查，若合法则允许注册
 */
void Register::on_button_register_clicked()
{
    //如果输入合法
    if (CheckInputs())
    {
        //数据库连接检查
        if (DataBase::instance()->IsOpen())
        {
            //连接数据库搜索当前注册输入
            QSqlQuery query = DataBase::instance()->Query();
            query.prepare(
                "select register(:username, :password, :phone, :mail, :address)"
            );
            query.bindValue(":username", ui->edit_name->text().trimmed());
            query.bindValue(":password", ui->edit_password->text().trimmed());
            query.bindValue(":phone", ui->edit_phone->text().trimmed());
            query.bindValue(":mail", ui->edit_mail->text().trimmed());
            query.bindValue(":address", ui->edit_address->toPlainText().trimmed());

            //如果成功搜索
            if (query.exec())
            {
                //如果成功返回
                if (query.first())
                {
                    //未注册则进行注册
                    if (query.value(0) == 1)
                    {
                        Clear();
                        QMessageBox::information(this, "提醒", "用户注册成功");
                        qDebug() << "Database query OK.";
                        this->hide();
                        return;
                    }//已存在该用户
                    else
                    {
                        QMessageBox::warning(this, "注意", "用户名重复");
                        qDebug() << "用户名重复";
                        return;
                    }
                }//注册失败
                else
                {
                    QMessageBox::warning(this, "注意", "用户注册出错");
                    qDebug() << query.lastError();
                    return;
                }
            }//注册失败
            else
            {
                QMessageBox::warning(this, "注意", "用户注册出错");
                qDebug() << query.lastError();
                return;
            }
        }//数据库连接失败
        else
        {
            QMessageBox::critical(this, "注意", "数据库连接断开！！！");
            qDebug() << "Database connection closed.";
            return;
        }
    }
}
