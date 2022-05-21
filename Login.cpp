#include "Login.h"
#include "ui_Login.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCloseEvent>

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

Login::~Login()
{
    delete ui;
}

void Login::Clear()
{
    ui->edit_password->clear();
    ui->edit_username->clear();
}

void Login::closeEvent (QCloseEvent *event)
{
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

void Login::on_button_login_clicked()
{
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

    QSqlQuery query = DataBase::instance()->Query();

    query.prepare(
        "SELECT login_in (:username, :password)"
    );
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec())
    {
        if (query.first())
        {
            if (query.value(0) > 0)
            {
                ui->label_status->setText("登录成功");
                DataBase::instance()->ChangePriority(query.value(0).toInt());
                this->hide();
                this->LoginIn(username);
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

void Login::on_button_register_clicked()
{
    Register *r = new Register(this);
    r->setModal(true);
    r->show();
}
