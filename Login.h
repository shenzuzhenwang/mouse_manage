/*********************************************************************
 * @file   Login.h
 * @brief  Login 类继承自QDialog，其对外暴露Login和Clear方法。
 *
 * @version 1.0
 * @author  陆畅
 * @date    2022.06.04
 *********************************************************************/
#ifndef Login_H
#define Login_H

#include <QDialog>

namespace Ui
{
class Login;
}

/**
 * @brief 登录界面类
 */
class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    void Clear();

signals:
    void LoginIn(QString name);

private slots:
    void on_button_login_clicked();

    void on_button_register_clicked();

private:
    Ui::Login *ui;

    void closeEvent (QCloseEvent *event);
};

#endif // Login_H
