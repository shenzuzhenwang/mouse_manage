/*********************************************************************
 * @file   Main.cpp
 * @brief 小鼠管理系统主页面，包含小鼠信息界面、查询界面、死亡列表界面、
 *        喂食记录页面、登录记录页面、用户信息界面
 *
 * @version 1.2
 * @author 王鹏博
 * @date   2022.06.04
 * @old_version 1.0
 * @old_author 王鹏博
 * @date   2022.03.05
 * @old_version 1.0
 * @old_author 王鹏博
 * @date   2022.01.01
 *********************************************************************/
#include "Main.h"
#include "ui_Main.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>
#include <QCloseEvent>

#include "DataBase.h"

/**
 * @brief 主界面初始化函数，用于初始化主界面的窗口设置，
 *        并且对数据库进行连接
 */
Main::Main(QWidget *parent) : QMainWindow(parent), ui(new Ui::Main)
{
    ui->setupUi(this);

    this->hide();

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // 窗口大小固定

    ui->birth_max->setDateTime(QDateTime::currentDateTime());
    ui->birth_min->setDateTime(QDateTime::currentDateTime());
    ui->lastmeal_max->setDateTime(QDateTime::currentDateTime());
    ui->lastmeal_min->setDateTime(QDateTime::currentDateTime());

    ui->table_mouse->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->table_mouse_death->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->table_feed_time->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->table_login->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //DataBase::instance()->SetupDB("root", "!zxc2010304052");
    DataBase::instance()->SetupDB();   //  创建一个连接至MySQL数据库

    if (DataBase::instance()->Connect())  // 数据库连接成功
    {
        QTextStream(stdout) << "Connect OK!";
    }
    else   // 数据库连接失败
    {
        QTextStream(stdout) << "Connect Fail!";
    }

    // 创建详细信息与功能界面、登录界面、帮助界面
    D_Mouse = new Mouse(this);
    D_Login = new Login(this);
    D_Help = new Help(this);

    // 关于
    connect(ui->act_about, &QAction::triggered, this, [this] {QMessageBox::information(this, "关于",
            "shenzu "
            "Welcomed to use "
            "email: shenzu@mail.nwpu.edu.cn");
                                                       });
    // 关于QT
    connect(ui->act_Qt, &QAction::triggered, this, [this] {QMessageBox::aboutQt(this);});
    // 帮助界面
    connect(ui->act_help, &QAction::triggered, D_Help, &Main::show);
    // 退出
    connect(ui->act_exit, &QAction::triggered, this,  &Main::close);
    // 注销
    connect(ui->act_logoff, &QAction::triggered, this,  &Main::log_off);
    // 刷新
    connect(ui->act_refresh, &QAction::triggered, this,  &Main::LoadMain);
    // 登录
    connect(D_Login, &Login::LoginIn, this, &Main::login_in);
    // 小鼠信息界面、喂食记录页面、死亡列表界面刷新
    connect(D_Mouse, &Mouse::RefreshMouse, this, &Main::LoadMouse);
    connect(D_Mouse, &Mouse::RefreshMouseDeath, this, &Main::LoadMouseDeath);
    connect(D_Mouse, &Mouse::RefreshFeed, this, &Main::LoadFeed);

    D_Login->show(); // 显示主界面
}

/*
 * @brief 析构界面
 */
Main::~Main()
{
    delete ui;
}

/*
 * @brief 主界面关闭，提示“确定退出吗”
 */
void Main::closeEvent (QCloseEvent *event)
{
    // 提示“确定退出吗”
    QMessageBox::StandardButton mboxSelect = QMessageBox::question(this, "退出", "确定退出吗",
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

    if (mboxSelect != QMessageBox::Yes)  // 不退出
    {
        event->ignore();
    }
    else  // 确认退出，先断开数据库连接，再进行关闭
    {
        DataBase::instance()->Disconnect();
        event->accept();
    }
}

/*
 * @brief 注销账户，显示登录界面
 */
void Main::log_off()
{
    this->hide();

    DataBase::instance()->ChangePriority(1);  // 将权限变为normal

    D_Login->show();
    D_Login->Clear();

}

/*
 * @brief 用户登录，通过查询数据库，判断密码是否正确，以及用户权限
 * @param name  用户名
 */
void Main::login_in(QString name)
{
    D_Login->hide();

    Clear();
    this->show();

    if (DataBase::instance()->GetPriority() == 1)  // 权限为normal
    {
        EnableChange(false);
        D_Mouse->EnableChange(false);
        if (ui->tw_main->count() == 6)
        {
            ui->tw_main->removeTab(5);
            ui->tw_main->removeTab(4);
        }
    }
    if (DataBase::instance()->GetPriority() == 2)  // 权限为senior
    {
        EnableChange(false);
        D_Mouse->EnableChange(true);
        if (ui->tw_main->count() == 6)
        {
            ui->tw_main->removeTab(5);
            ui->tw_main->removeTab(4);
        }
    }
    if (DataBase::instance()->GetPriority() == 3)  // 权限为admin
    {
        EnableChange(true);
        D_Mouse->EnableChange(true);
        if (ui->tw_main->count() == 4)  // 显示用户信息表、登录记录表
        {
            ui->tw_main->insertTab(4, ui->tab_user, "用户信息表");
            ui->tw_main->insertTab(5, ui->tab_login, "登录记录表");
        }
    }

    if (DataBase::instance()->IsOpen())  // 数据库连接正常，则加载主界面
    {
        qDebug() << "Connect OK!";

        LoadMain();
    }
    else
    {
        QMessageBox::critical(this, "警告", DataBase::instance()->GetError());
        QTextStream(stdout) << "Connect Fail!";
    }

    D_Mouse->username = name;
    ui->label_welcome->setText("Welcome, " + name);
}

/*
 * @brief 设置主界面中的表格，是否可以被直接更改
 * @param arg true 可以被更改；false 不可被更改
 */
void Main::EnableChange(bool arg)
{
    // 激活或失活死亡列表和喂食列表的更改按钮
    if (arg == true)
    {
        ui->table_mouse_death->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
        ui->table_feed_time->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
    }
    else
    {
        ui->table_mouse_death->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->table_feed_time->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    // 激活或失活更改按钮
    ui->btn_cm_death->setEnabled(arg);
    ui->btn_ad_death->setEnabled(arg);
    ui->btn_de_death->setEnabled(arg);
    ui->btn_rb_death->setEnabled(arg);
    ui->btn_cm_feed->setEnabled(arg);
    ui->btn_ad_feed->setEnabled(arg);
    ui->btn_de_feed->setEnabled(arg);
    ui->btn_rb_feed->setEnabled(arg);
}

/*
 * @brief 加载主页面中的各项信息
 */
void Main::LoadMain()
{
    // 加载小鼠信息界面、死亡列表界面、喂食记录页面
    LoadMouse();
    LoadMouseDeath();
    LoadFeed();
    if (DataBase::instance()->GetPriority() == 3)  // 如果权限为admin，加载登录记录页面、用户信息界面
    {
        LoadUser();
        LoadLogin();
    }
}

/*
 * @brief 加载小鼠信息界面中的各项信息
 */
void Main::LoadMouse()
{
    // 执行一个sql语句"SELECT name FROM user"，从用户信息表中找到用户名
    QSqlQuery query = QSqlQuery(DataBase::instance()->GetDatabase());

    query.exec("SELECT name FROM user");

    while (query.next())
    {
        if (query.isActive())
        {
            QString name = query.value(0).toString();
            if (name != "admin")
                ui->box_master->addItem(name);
        }
    }

    // 执行一个sql语句"SELECT * FROM mouse"，从小鼠信息表中找到小鼠的所有信息，并加入GUI的表格中
    query.prepare(
        "SELECT * FROM mouse"
    );

    if (query.exec())
    {
        if (query.isActive())
        {
            qDebug() << "load mouse OK";

            QSqlQueryModel * queryModel = new QSqlQueryModel(this);
            queryModel->setQuery(std::move(query));

            queryModel->setHeaderData(0, Qt::Horizontal, "ID");
            queryModel->setHeaderData(1, Qt::Horizontal, "Name");
            queryModel->setHeaderData(2, Qt::Horizontal, "Description");
            queryModel->setHeaderData(3, Qt::Horizontal, "Health");
            queryModel->setHeaderData(4, Qt::Horizontal, "Master Name");
            queryModel->setHeaderData(5, Qt::Horizontal, "Birth Time");
            queryModel->setHeaderData(6, Qt::Horizontal, "Last Meal Time");

            // 将信息加入GUI的表格中
            ui->table_mouse->setModel(queryModel);
            ui->statusbar->showMessage("load mouse success: " + QString("%1").arg(queryModel->rowCount()));
        }
        else
        {
            QMessageBox::warning(this, "注意", "加载信息出错");
            qDebug() << query.lastError();
            return;
        }
    }
    else
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << query.lastError();
        return;
    }
}

/*
 * @brief 加载小鼠死亡信息界面中的各项信息
 */
void Main::LoadMouseDeath()
{
    M_mouse_death = new QSqlTableModel(this, DataBase::instance()->GetDatabase());
    // 将信息加入GUI的表格中
    M_mouse_death->setTable("mouse_death");
    if (M_mouse_death->select() == false)
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << M_mouse_death->lastError();
        return;
    }

    ui->table_mouse_death->setModel(M_mouse_death);

    qDebug() << "Set Model";
}

/*
 * @brief 加载小鼠喂食信息界面中的各项信息
 */
void Main::LoadFeed()
{
    M_feed = new QSqlTableModel(this, DataBase::instance()->GetDatabase());
    // 将信息加入GUI的表格中
    M_feed->setTable("feed");
    if (M_feed->select() == false)
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << M_feed->lastError();
        return;
    }

    ui->table_feed_time->setModel(M_feed);
}

/*
 * @brief 加载用户信息界面中的各项信息
 */
void Main::LoadUser()
{
    M_user = new QSqlTableModel(this, DataBase::instance()->GetDatabase());
    // 将信息加入GUI的表格中
    M_user->setTable("user");
    if (M_user->select() == false)
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << M_user->lastError();
        return;
    }

    ui->table_user->setModel(M_user);
    ui->table_user->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

/*
 * @brief 加载用户信息界面中的各项信息
 */
void Main::LoadLogin()
{
    M_login = new QSqlTableModel(this, DataBase::instance()->GetDatabase());
    // 将信息加入GUI的表格中
    M_login->setTable("login");
    if (M_login->select() == false)
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << M_login->lastError();
        return;
    }

    ui->table_login->setModel(M_login);
}

/*
 * @brief 加载小鼠信息按钮
 */
void Main::on_btn_rf_mouse_clicked()
{
    LoadMouse();
}

/*
 * @brief 清除小鼠信息按钮
 */
void Main::on_btn_reset_clicked()
{
    Clear();
}

/*
 * @brief 编辑小鼠信息按钮，进入详细信息与功能界面
 */
void Main::on_btn_edit_clicked()
{
    D_Mouse->setModal(true);
    D_Mouse->show();
    D_Mouse->Clear();
}

/*
 * @brief 小鼠信息表格双击按钮，进入详细信息与功能界面
 */
void Main::on_table_mouse_activated(const QModelIndex &index)
{
    D_Mouse->setModal(true);
    D_Mouse->show();
    D_Mouse->LoadMouse(index.sibling(index.row(), 0).data().toString());
}

/*
 * @brief 小鼠信息表格按下按钮，显示具体信息
 */
void Main::on_table_mouse_pressed(const QModelIndex &index)
{
    ui->label_message->setText("ID: " + index.sibling(index.row(), 0).data().toString() + " - "
                               + index.model()->headerData(index.column(), Qt::Horizontal).toString() + ": " + index.data().toString());
}

/*
 * @brief 查询界面，点击查询按钮，显示查询结果
 */
void Main::on_btn_search_clicked()
{
    // 创建sql语句
    QString queryString = "SELECT * FROM mouse ";
    QString queryStringWhereConcatenator = "WHERE ";

    // 检测是否按名字查询
    if (ui->check_name->checkState())
    {
        QString productName = ui->edit_name->text().trimmed();
        if (!productName.isEmpty())
        {
            // 设置查询的方式
            queryString += queryStringWhereConcatenator;
            if (ui->box_name->currentIndex() == 0)   //Exactly
            {
                queryString += "name = '" + productName + "' ";
            }
            else if (ui->box_name->currentIndex() == 1)    //Stars With
            {
                queryString += "name like '" + productName + "%' ";
            }
            else if (ui->box_name->currentIndex() == 2)    //Ends With
            {
                queryString += "name like '%" + productName + "' ";
            }
            queryStringWhereConcatenator = "AND ";
        }
        else
        {
            QMessageBox::warning(this, "Search", "SearchByName option selected but text is null or empty!");
            return;
        }
    }
    // 检测是否按出生日期查询
    if (ui->check_birth->checkState())
    {
        queryString += queryStringWhereConcatenator;

        QString time_format = "yyyy-MM-dd  HH:mm:ss";
        QString smin = ui->birth_min->dateTime().toString(time_format);
        QString smax = ui->birth_max->dateTime().toString(time_format);
        if (smin > smax)
            swap(smin, smax);

        queryString += "birth BETWEEN '" + smin + "' AND '" + smax + "' ";
        queryStringWhereConcatenator = "AND ";
    }
    // 检测是否按喂食时间查询
    if (ui->check_lastmeal->checkState())
    {
        queryString += queryStringWhereConcatenator;

        QString time_format = "yyyy-MM-dd  HH:mm:ss";
        QString smin = ui->lastmeal_min->dateTime().toString(time_format);
        QString smax = ui->lastmeal_max->dateTime().toString(time_format);
        if (smin > smax)
            swap(smin, smax);

        queryString += "lastmeal BETWEEN '" + smin + "' AND '" + smax + "' ";
        queryStringWhereConcatenator = "AND ";
    }
    // 检测是否按健康情况查询
    if (ui->check_health->checkState())
    {
        queryString += queryStringWhereConcatenator;

        queryString += "health = '" + ui->box_health->currentText() + "' ";

        queryStringWhereConcatenator = "AND ";
    }
    // 检测是否按主人名称查询
    if (ui->check_master->checkState())
    {
        queryString += queryStringWhereConcatenator;

        queryString += "master = '" + ui->box_master->currentText() + "' ";

        queryStringWhereConcatenator = "AND ";
    }

    qDebug() << "Search Query: " << queryString;

    QSqlQuery query = QSqlQuery(DataBase::instance()->GetDatabase());
    query.prepare(queryString);

    // 将数据库查询的结果，显示在GUI的表格中
    if (query.exec())
    {
        if (query.isActive())
        {
            qDebug() << "Search OK";

            QSqlQueryModel * queryModel = new QSqlQueryModel(this);
            queryModel->setQuery(std::move(query));

            queryModel->setHeaderData(0, Qt::Horizontal, "ID");
            queryModel->setHeaderData(1, Qt::Horizontal, "Barcode");
            queryModel->setHeaderData(2, Qt::Horizontal, "Name");
            queryModel->setHeaderData(3, Qt::Horizontal, "Description");
            queryModel->setHeaderData(4, Qt::Horizontal, "Quantity");
            queryModel->setHeaderData(5, Qt::Horizontal, "Purchase Price");
            queryModel->setHeaderData(6, Qt::Horizontal, "Sale Price");
            queryModel->setHeaderData(7, Qt::Horizontal, "Add Date");
            queryModel->setHeaderData(8, Qt::Horizontal, "Update Date");

            ui->table_mouse->setModel(queryModel);

            ui->tw_main->setCurrentIndex(0);

            ui->statusbar->showMessage("Search() success: " + QString("%1").arg(queryModel->rowCount()));
        }
        else
        {
            QMessageBox::warning(this, "注意", "查找出错!");
            qDebug() << query.lastError();
            return;
        }
    }
    else
    {
        QMessageBox::warning(this, "注意", "查找出错!");
        qDebug() << query.lastError();
        return;
    }
}

/*
 * @brief 主界面中数据进行清除
 */
void Main::Clear()
{
    ui->edit_name->setEnabled(false);
    ui->box_name->setEnabled(false);
    ui->birth_max->setEnabled(false);
    ui->birth_min->setEnabled(false);
    ui->lastmeal_max->setEnabled(false);
    ui->lastmeal_min->setEnabled(false);
    ui->box_master->setEnabled(false);
    ui->box_health->setEnabled(false);

    ui->edit_name->clear();
    ui->box_name->setCurrentIndex(0);

    ui->birth_max->setDateTime(QDateTime::currentDateTime());
    ui->birth_min->setDateTime(QDateTime::currentDateTime());

    ui->lastmeal_max->setDateTime(QDateTime::currentDateTime());
    ui->lastmeal_min->setDateTime(QDateTime::currentDateTime());

    ui->box_master->setCurrentIndex(0);
    ui->box_health->setCurrentIndex(0);

    ui->check_name->setCheckState(Qt::Unchecked);
    ui->check_birth->setCheckState(Qt::Unchecked);
    ui->check_health->setCheckState(Qt::Unchecked);
    ui->check_master->setCheckState(Qt::Unchecked);
    ui->check_lastmeal->setCheckState(Qt::Unchecked);
}

/*
 * @brief 名字查询情况更改
 */
void Main::on_check_name_stateChanged(int arg1)
{
    ui->edit_name->setEnabled(arg1);
    ui->box_name->setEnabled(arg1);
}

/*
 * @brief 出生日期查询情况更改
 */
void Main::on_check_birth_stateChanged(int arg1)
{
    ui->birth_max->setEnabled(arg1);
    ui->birth_min->setEnabled(arg1);
}

/*
 * @brief 健康情况查询情况更改
 */
void Main::on_check_health_stateChanged(int arg1)
{
    ui->box_health->setEnabled(arg1);
}

/*
 * @brief 主人名称查询情况更改
 */
void Main::on_check_master_stateChanged(int arg1)
{
    ui->box_master->setEnabled(arg1);
}

/*
 * @brief 喂食记录查询情况更改
 */
void Main::on_check_lastmeal_stateChanged(int arg1)
{
    ui->lastmeal_max->setEnabled(arg1);
    ui->lastmeal_min->setEnabled(arg1);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Main w;

    return a.exec();
}

/*
 * @brief 死亡列表信息提交按钮
 */
void Main::on_btn_cm_death_clicked()
{
    M_mouse_death->database().transaction();
    if (M_mouse_death->submitAll())
    {
        M_mouse_death->database().commit(); //提交
    }
    else
    {
        M_mouse_death->database().rollback(); //回滚
        QMessageBox::warning(this, "注意", M_mouse_death->lastError().text());
    }
}

/*
 * @brief 死亡列表信息回滚按钮
 */
void Main::on_btn_rb_death_clicked()
{
    M_mouse_death->revertAll();
}

/*
 * @brief 死亡列表信息加一行按钮
 */
void Main::on_btn_ad_death_clicked()
{
    int rowNum = M_mouse_death->rowCount();
    M_mouse_death->insertRow(rowNum);
}

/*
 * @brief 死亡列表信息删除一行按钮
 */
void Main::on_btn_de_death_clicked()
{
    int curRow = ui->table_mouse_death->currentIndex().row();
    M_mouse_death->removeRow(curRow);
}

/*
 * @brief 死亡列表信息刷新按钮
 */
void Main::on_btn_rf_death_clicked()
{
    LoadMouseDeath();
}

/*
 * @brief 喂食记录列表信息提交按钮
 */
void Main::on_btn_cm_feed_clicked()
{
    M_feed->database().transaction();
    if (M_feed->submitAll())
    {
        M_feed->database().commit(); //提交
    }
    else
    {
        M_feed->database().rollback(); //回滚
        QMessageBox::warning(this, "注意", M_feed->lastError().text());
    }
}

/*
 * @brief 喂食记录列表信息回滚按钮
 */
void Main::on_btn_rb_feed_clicked()
{
    M_feed->revertAll();
}

/*
 * @brief 喂食记录列表信息加一行按钮
 */
void Main::on_btn_ad_feed_clicked()
{
    int rowNum = M_feed->rowCount();
    M_feed->insertRow(rowNum);
}

/*
 * @brief 喂食记录列表信息删除一行按钮
 */
void Main::on_btn_de_feed_clicked()
{
    int curRow = ui->table_feed_time->currentIndex().row();
    M_feed->removeRow(curRow);
}

/*
 * @brief 喂食记录列表信息刷新按钮
 */
void Main::on_btn_rf_feed_clicked()
{
    LoadFeed();
}

/*
 * @brief 登录记录列表信息提交按钮
 */
void Main::on_btn_cm_login_clicked()
{
    M_login->database().transaction();
    if (M_login->submitAll())
    {
        M_login->database().commit(); //提交
    }
    else
    {
        M_login->database().rollback(); //回滚
        QMessageBox::warning(this, "注意", M_login->lastError().text());
    }
}
/*
 * @brief 登录记录列表信息回滚按钮
 */
void Main::on_btn_rb_login_clicked()
{
    M_login->revertAll();
}

/*
 * @brief 登录记录列表信息加一行按钮
 */
void Main::on_btn_ad_login_clicked()
{
    int rowNum = M_login->rowCount();
    M_login->insertRow(rowNum);
}

/*
 * @brief 登录记录列表信息删除一行按钮
 */
void Main::on_btn_de_login_clicked()
{
    int curRow = ui->table_login->currentIndex().row();
    M_login->removeRow(curRow);
}

/*
 * @brief 登录记录列表信息刷新按钮
 */
void Main::on_btn_rf_login_clicked()
{
    LoadLogin();
}

/*
 * @brief 用户信息列表信息提交按钮
 */
void Main::on_btn_cm_user_clicked()
{
    M_user->database().transaction();
    if (M_user->submitAll())
    {
        M_user->database().commit(); //提交
    }
    else
    {
        M_user->database().rollback(); //回滚
        QMessageBox::warning(this, "注意", M_user->lastError().text());
    }
}

/*
 * @brief 用户信息列表信息回滚按钮
 */
void Main::on_btn_rb_user_clicked()
{
    M_user->revertAll();
}

/*
 * @brief 用户信息列表信息加一行按钮
 */
void Main::on_btn_ad_user_clicked()
{
    int rowNum = M_user->rowCount();
    M_user->insertRow(rowNum);
}

/*
 * @brief 用户信息列表信息删除一行按钮
 */
void Main::on_btn_de_user_clicked()
{
    int curRow = ui->table_user->currentIndex().row();
    M_user->removeRow(curRow);
}

/*
 * @brief 用户信息列表信息刷新按钮
 */
void Main::on_btn_rf_user_clicked()
{
    LoadUser();
}
