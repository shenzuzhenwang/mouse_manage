#include "Main.h"
#include "ui_Main.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>
#include <QCloseEvent>

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
    DataBase::instance()->SetupDB();

    if (DataBase::instance()->Connect())
    {
        QTextStream(stdout) << "Connect OK!";
    }
    else
    {
        QTextStream(stdout) << "Connect Fail!";
    }

    D_Mouse = new Mouse(this);
    D_Login = new Login(this);
    D_Help = new Help(this);

    // 关于
    connect(ui->act_about, &QAction::triggered, [this] {QMessageBox::information(this, "关于",
            "shenzu "
            "Welcomed to use "
            "email: shenzu@mail.nwpu.edu.cn");
                                                       });
    // 关于QT
    connect(ui->act_Qt, &QAction::triggered, [this] {QMessageBox::aboutQt(this);});
    // 帮助界面
    connect(ui->act_help, &QAction::triggered, D_Help, &Main::show);
    // 退出
    connect(ui->act_exit, &QAction::triggered, this,  &Main::close);
    // 注销
    connect(ui->act_logoff, &QAction::triggered, this,  &Main::log_off);
    // 刷新
    connect(ui->act_refresh, &QAction::triggered, this,  &Main::LoadMain);

    connect(D_Login, &Login::LoginIn, this, &Main::login_in);

    connect(D_Mouse, &Mouse::RefreshMouse, this, &Main::LoadMouse);
    connect(D_Mouse, &Mouse::RefreshMouseDeath, this, &Main::LoadMouseDeath);
    connect(D_Mouse, &Mouse::RefreshFeed, this, &Main::LoadFeed);

    D_Login->show();
}

Main::~Main()
{
    delete ui;
}

void Main::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton mboxSelect = QMessageBox::question(this, "退出", "确定退出吗",
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

    if (mboxSelect != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        DataBase::instance()->Disconnect();
        event->accept();
    }
}

void Main::log_off()
{
    this->hide();

    DataBase::instance()->ChangePriority(1);

    D_Login->show();
    D_Login->Clear();

}

void Main::login_in(QString name)
{
    D_Login->hide();

    Clear();
    this->show();

    if (DataBase::instance()->GetPriority() == 1)
    {
        EnableChange(false);
        D_Mouse->EnableChange(false);
        if (ui->tw_main->count() == 6)
        {
            ui->tw_main->removeTab(5);
            ui->tw_main->removeTab(4);
        }
    }
    if (DataBase::instance()->GetPriority() == 2)
    {
        EnableChange(false);
        D_Mouse->EnableChange(true);
        if (ui->tw_main->count() == 6)
        {
            ui->tw_main->removeTab(5);
            ui->tw_main->removeTab(4);
        }
    }
    if (DataBase::instance()->GetPriority() == 3)
    {
        EnableChange(true);
        D_Mouse->EnableChange(true);
        if (ui->tw_main->count() == 4)
        {
            ui->tw_main->insertTab(4, ui->tab_user, "用户信息表");
            ui->tw_main->insertTab(5, ui->tab_login, "登录记录表");
        }
    }

    if (DataBase::instance()->IsOpen())
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

void Main::EnableChange(bool arg)
{
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

    ui->btn_cm_death->setEnabled(arg);
    ui->btn_ad_death->setEnabled(arg);
    ui->btn_de_death->setEnabled(arg);
    ui->btn_rb_death->setEnabled(arg);
    ui->btn_cm_feed->setEnabled(arg);
    ui->btn_ad_feed->setEnabled(arg);
    ui->btn_de_feed->setEnabled(arg);
    ui->btn_rb_feed->setEnabled(arg);
}

void Main::LoadMain()
{
    LoadMouse();
    LoadMouseDeath();
    LoadFeed();
    if (DataBase::instance()->GetPriority() == 3)
    {
        LoadUser();
        LoadLogin();
    }
}
void Main::LoadMouse()
{
    QSqlQueryModel * queryModel = new QSqlQueryModel(this);

    QSqlQuery query = DataBase::instance()->Query();

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

    query.prepare(
        "SELECT * FROM mouse"
    );

    if (query.exec())
    {
        if (query.isActive())
        {
            qDebug() << "load mouse OK";
            queryModel->setQuery(query);

            queryModel->setHeaderData(0, Qt::Horizontal, "ID");
            queryModel->setHeaderData(1, Qt::Horizontal, "Name");
            queryModel->setHeaderData(2, Qt::Horizontal, "Description");
            queryModel->setHeaderData(3, Qt::Horizontal, "Health");
            queryModel->setHeaderData(4, Qt::Horizontal, "Master Name");
            queryModel->setHeaderData(5, Qt::Horizontal, "Birth Time");
            queryModel->setHeaderData(6, Qt::Horizontal, "Last Meal Time");

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

void Main::LoadMouseDeath()
{
    M_mouse_death = new QSqlTableModel(this, DataBase::instance()->GetDatabase());

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

void Main::LoadFeed()
{
    M_feed = new QSqlTableModel(this, DataBase::instance()->GetDatabase());

    M_feed->setTable("feed");
    if (M_feed->select() == false)
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << M_feed->lastError();
        return;
    }

    ui->table_feed_time->setModel(M_feed);
}

void Main::LoadUser()
{
    M_user = new QSqlTableModel(this, DataBase::instance()->GetDatabase());

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

void Main::LoadLogin()
{
    M_login = new QSqlTableModel(this, DataBase::instance()->GetDatabase());

    M_login->setTable("login");
    if (M_login->select() == false)
    {
        QMessageBox::warning(this, "注意", "加载信息出错");
        qDebug() << M_login->lastError();
        return;
    }

    ui->table_login->setModel(M_login);
}


void Main::on_btn_rf_mouse_clicked()
{
    LoadMouse();
}

void Main::on_btn_reset_clicked()
{
    Clear();
}

void Main::on_btn_edit_clicked()
{
    D_Mouse->setModal(true);
    D_Mouse->show();
    D_Mouse->Clear();
}

void Main::on_table_mouse_activated(const QModelIndex &index)
{
    D_Mouse->setModal(true);
    D_Mouse->show();
    D_Mouse->LoadMouse(index.sibling(index.row(), 0).data().toString());
}

void Main::on_table_mouse_pressed(const QModelIndex &index)
{
    ui->label_message->setText("ID: " + index.sibling(index.row(), 0).data().toString() + " - "
                               + index.model()->headerData(index.column(), Qt::Horizontal).toString() + ": " + index.data().toString());
}

void Main::on_btn_search_clicked()
{
    QString queryString = "SELECT * FROM mouse ";
    QString queryStringWhereConcatenator = "WHERE ";

    if (ui->check_name->checkState())
    {
        QString productName = ui->edit_name->text().trimmed();
        if (!productName.isEmpty())
        {
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

    if (ui->check_health->checkState())
    {
        queryString += queryStringWhereConcatenator;

        queryString += "health = '" + ui->box_health->currentText() + "' ";

        queryStringWhereConcatenator = "AND ";
    }

    if (ui->check_master->checkState())
    {
        queryString += queryStringWhereConcatenator;

        queryString += "master = '" + ui->box_master->currentText() + "' ";

        queryStringWhereConcatenator = "AND ";
    }

    qDebug() << "Search Query: " << queryString;

    QSqlQuery query = DataBase::instance()->Query();
    query.prepare(queryString);

    QSqlQueryModel * queryModel = new QSqlQueryModel(this);

    if (query.exec())
    {
        if (query.isActive())
        {
            qDebug() << "Search OK";
            queryModel->setQuery(query);

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

void Main::on_check_name_stateChanged(int arg1)
{
    ui->edit_name->setEnabled(arg1);
    ui->box_name->setEnabled(arg1);
}

void Main::on_check_birth_stateChanged(int arg1)
{
    ui->birth_max->setEnabled(arg1);
    ui->birth_min->setEnabled(arg1);
}

void Main::on_check_health_stateChanged(int arg1)
{
    ui->box_health->setEnabled(arg1);
}

void Main::on_check_master_stateChanged(int arg1)
{
    ui->box_master->setEnabled(arg1);
}

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

void Main::on_btn_rb_death_clicked()
{
    M_mouse_death->revertAll();
}

void Main::on_btn_ad_death_clicked()
{
    int rowNum = M_mouse_death->rowCount();
    M_mouse_death->insertRow(rowNum);
}

void Main::on_btn_de_death_clicked()
{
    int curRow = ui->table_mouse_death->currentIndex().row();
    M_mouse_death->removeRow(curRow);
}

void Main::on_btn_rf_death_clicked()
{
    LoadMouseDeath();
}

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

void Main::on_btn_rb_feed_clicked()
{
    M_feed->revertAll();
}

void Main::on_btn_ad_feed_clicked()
{
    int rowNum = M_feed->rowCount();
    M_feed->insertRow(rowNum);
}

void Main::on_btn_de_feed_clicked()
{
    int curRow = ui->table_feed_time->currentIndex().row();
    M_feed->removeRow(curRow);
}

void Main::on_btn_rf_feed_clicked()
{
    LoadFeed();
}

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

void Main::on_btn_rb_login_clicked()
{
    M_login->revertAll();
}

void Main::on_btn_ad_login_clicked()
{
    int rowNum = M_login->rowCount();
    M_login->insertRow(rowNum);
}

void Main::on_btn_de_login_clicked()
{
    int curRow = ui->table_login->currentIndex().row();
    M_login->removeRow(curRow);
}

void Main::on_btn_rf_login_clicked()
{
    LoadLogin();
}

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

void Main::on_btn_rb_user_clicked()
{
    M_user->revertAll();
}

void Main::on_btn_ad_user_clicked()
{
    int rowNum = M_user->rowCount();
    M_user->insertRow(rowNum);
}

void Main::on_btn_de_user_clicked()
{
    int curRow = ui->table_user->currentIndex().row();
    M_user->removeRow(curRow);
}

void Main::on_btn_rf_user_clicked()
{
    LoadUser();
}
