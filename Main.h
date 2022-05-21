#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QSqlTableModel>
#include "Mouse.h"
#include "Login.h"
#include "Help.h"
#include "DataBase.h"

namespace Ui
{
class Main;
}

class Main : public QMainWindow
{
    Q_OBJECT

private:
    Mouse *D_Mouse;
    Login *D_Login;
    Help *D_Help;

    QSqlTableModel *M_mouse_death;
    QSqlTableModel *M_feed;
    QSqlTableModel *M_user;
    QSqlTableModel *M_login;

public:
    explicit Main(QWidget *parent = 0);
    ~Main();

private slots:

    void login_in(QString name);

    void log_off();

    void on_table_mouse_activated(const QModelIndex &index);

    void on_table_mouse_pressed(const QModelIndex &index);

    void on_btn_edit_clicked();

    void on_check_name_stateChanged(int arg1);

    void on_check_birth_stateChanged(int arg1);

    void on_check_health_stateChanged(int arg1);

    void on_check_master_stateChanged(int arg1);

    void on_check_lastmeal_stateChanged(int arg1);

    void on_btn_search_clicked();

    void on_btn_rf_mouse_clicked();

    void on_btn_rf_death_clicked();

    void on_btn_rf_feed_clicked();

    void on_btn_rf_login_clicked();

    void on_btn_rf_user_clicked();

    void on_btn_reset_clicked();

    void on_btn_cm_death_clicked();

    void on_btn_rb_death_clicked();

    void on_btn_ad_death_clicked();

    void on_btn_de_death_clicked();

    void on_btn_cm_feed_clicked();

    void on_btn_rb_feed_clicked();

    void on_btn_ad_feed_clicked();

    void on_btn_de_feed_clicked();

    void on_btn_cm_login_clicked();

    void on_btn_rb_login_clicked();

    void on_btn_ad_login_clicked();

    void on_btn_de_login_clicked();

    void on_btn_cm_user_clicked();

    void on_btn_rb_user_clicked();

    void on_btn_ad_user_clicked();

    void on_btn_de_user_clicked();

private:
    Ui::Main *ui;

    void LoadMain();
    void LoadMouse();
    void LoadMouseDeath();
    void LoadFeed();
    void LoadUser();
    void LoadLogin();

    void Clear();

    void EnableChange(bool arg);

    void closeEvent(QCloseEvent *event);

};

#endif // FRMMAIN_H
