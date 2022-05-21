#include "Register.h"
#include "ui_Register.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QRegExp>

Register::Register(QWidget *parent) : QDialog(parent), ui(new Ui::Register)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // 窗口大小固定
}

Register::~Register()
{
    delete ui;
}

bool Register::CheckInputs()
{
    QRegExp regexMail("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    regexMail.setCaseSensitivity(Qt::CaseInsensitive);  // 大小写不敏感

    QRegExp regexPhone("^[0-9]{8,11}$");
    regexPhone.setCaseSensitivity(Qt::CaseInsensitive);

    if (ui->edit_name->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "名字不能为空");
        return false;
    }
    else if (ui->edit_password->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "密码不能为空");
        return false;
    }
    else if (ui->edit_password2->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "重复密码不能为空");
        return false;
    }
    else if (QString::compare(ui->edit_password->text().trimmed(), ui->edit_password2->text().trimmed()) != 0)
    {
        QMessageBox::warning(this, "注意", "重复密码与密码不符");
        return false;
    }
    else if (ui->edit_mail->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "邮箱不能为空");
        return false;
    }
    else if (!regexMail.exactMatch(ui->edit_mail->text().trimmed()))
    {
        QMessageBox::warning(this, "注意", "邮箱格式不符");
        return false;
    }
    else if (ui->edit_phone->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "电话不能为空");
        return false;
    }
    else if (!regexPhone.exactMatch(ui->edit_phone->text().trimmed()))
    {
        QMessageBox::warning(this, "注意", "电话号码长度应为8-11");
        return false;
    }
    else if (!ui->check_service->checkState())
    {
        QMessageBox::warning(this, "注意", "需要同意协议!");
        return false;
    }

    return true;
}

void Register::Clear()
{
    ui->edit_name->clear();
    ui->edit_password->clear();
    ui->edit_password2->clear();
    ui->edit_mail->clear();
    ui->edit_phone->clear();
    ui->edit_address->clear();
}

void Register::on_button_register_clicked()
{
    if (CheckInputs())
    {
        if (DataBase::instance()->IsOpen())
        {
            QSqlQuery query = DataBase::instance()->Query();
            query.prepare(
                "select register(:username, :password, :phone, :mail, :address)"
            );
            query.bindValue(":username", ui->edit_name->text().trimmed());
            query.bindValue(":password", ui->edit_password->text().trimmed());
            query.bindValue(":phone", ui->edit_phone->text().trimmed());
            query.bindValue(":mail", ui->edit_mail->text().trimmed());
            query.bindValue(":address", ui->edit_address->toPlainText().trimmed());

            if (query.exec())
            {
                if (query.first())
                {
                    if (query.value(0) == 1)
                    {
                        Clear();
                        QMessageBox::information(this, "提醒", "用户注册成功");
                        qDebug() << "Database query OK.";
                        this->hide();
                        return;
                    }
                    else
                    {
                        QMessageBox::warning(this, "注意", "用户名重复");
                        qDebug() << "用户名重复";
                        return;
                    }
                }
                else
                {
                    QMessageBox::warning(this, "注意", "用户注册出错");
                    qDebug() << query.lastError();
                    return;
                }
            }
            else
            {
                QMessageBox::warning(this, "注意", "用户注册出错");
                qDebug() << query.lastError();
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, "注意", "数据库连接断开！！！");
            qDebug() << "Database connection closed.";
            return;
        }
    }
}
