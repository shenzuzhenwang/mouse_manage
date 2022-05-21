#include "Mouse.h"
#include "ui_Mouse.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

Mouse::Mouse(QWidget *parent) : QDialog(parent), ui(new Ui::Mouse)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // 窗口大小固定

    Clear();

    LoadToComboBox();
    ui->time_birth->setDateTime(QDateTime::currentDateTime());
    ui->time_lastmeal->setDateTime(QDateTime::currentDateTime());
}

Mouse::~Mouse()
{
    delete ui;
}


void Mouse::Clear()
{
    ui->box_ID->setCurrentText(" ");
    ui->edit_id->clear();
    ui->edit_name->clear();
    ui->edit_description->clear();
    ui->check_readonly->setChecked(true);
    ui->time_birth->setDateTime(QDateTime::currentDateTime());
    ui->time_lastmeal->setDateTime(QDateTime::currentDateTime());
}

bool Mouse::CheckInputs()
{
    if (ui->edit_id->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "id不能为空!");
        return false;
    }
    return true;
}

void Mouse::LoadToComboBox()
{
    QSqlQuery query = DataBase::instance()->Query();
    query.exec("SELECT id FROM mouse ORDER BY id");

    while (query.next())
    {
        if (query.isActive())
        {
            QString id = query.value(0).toString();
            ui->box_ID->addItem(id);
        }
    }
    ui->box_ID->addItem(" ");

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

    ui->box_health->addItem("健康");
    ui->box_health->addItem("良好");
    ui->box_health->addItem("生病");
    ui->box_health->addItem("极差");
}

void Mouse::LoadMouse(const QString id)
{
    QSqlQuery query = DataBase::instance()->Query();
    query.prepare(
        "SELECT * FROM "
        "mouse "
        "WHERE "
        "id = :id"
    );

    query.bindValue(":id", id.toInt());

    if (query.exec())
    {
        if (query.first())
        {
            if (query.isActive())
            {
                ui->box_ID->setCurrentText(id);
                ui->edit_id->setText(query.value(0).toString());
                ui->edit_name->setText(query.value(1).toString());
                ui->edit_description->setText(query.value(2).toString());
                ui->box_health->setCurrentText(query.value(3).toString());
                ui->box_master->setCurrentText(query.value(4).toString());
                ui->time_birth->setDateTime(query.value(5).toDateTime());
                ui->time_lastmeal->setDateTime(query.value(6).toDateTime());
                qDebug() << "Load Mouse: " << id;
            }
        }
        else
        {
            Clear();
        }
    }
    else
    {
        QMessageBox::warning(this, "注意", "查找失败!");
        qDebug() << query.lastError();
        return;
    }
}

void Mouse::on_button_add_clicked()
{
    if (CheckInputs())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定出生吗？", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            return;
        }
        else if (reply == QMessageBox::Yes)
        {
            if (DataBase::instance()->IsOpen())
            {
                QSqlQuery query = DataBase::instance()->Query();

                query.prepare(
                    "select mouse_birth (:id, :name, :master, :health, :description)"
                );

                query.bindValue(":id", ui->edit_id->text().trimmed().toInt());
                query.bindValue(":name", ui->edit_name->text().trimmed());
                query.bindValue(":master", ui->box_master->currentText());
                query.bindValue(":health", ui->box_health->currentText());
                query.bindValue(":description", ui->edit_description->toPlainText().trimmed());

                if (query.exec())
                {
                    if (query.first())
                    {
                        if (query.value(0) == 1)
                        {
                            QMessageBox::information(this, "提示", "出生记录成功!");
                            qDebug() << "Database query OK." << query.lastQuery();
                            ui->box_ID->insertItem(ui->box_ID->count() - 1, ui->edit_id->text().trimmed());
                            Clear();
                            RefreshMouse();
                            return;
                        }
                        else
                        {
                            QMessageBox::warning(this, "注意", "id编号重复!");
                            qDebug() << "id编号重复";
                            return;
                        }
                    }
                    else
                    {
                        QMessageBox::warning(this, "注意", "出生记录失败!");
                        qDebug() << query.lastError();
                        return;
                    }
                }
                else
                {
                    QMessageBox::warning(this, "注意", "出生记录失败!");
                    qDebug() << query.lastError();
                    return;
                }
            }
            else
            {
                QMessageBox::critical(this, "警告", "数据库连接断开!");
                qDebug() << "Database connection closed.";
                return;
            }
        }
    }
}

void Mouse::on_button_update_clicked()
{
    if (QString::compare(ui->edit_id->text().trimmed(), ui->box_ID->currentText(), Qt::CaseSensitive) != 0)
    {
        QMessageBox::warning(this, "注意", "更新的id应相同!");
        ui->edit_id->setText(ui->box_ID->currentText());
        return;
    }

    if (CheckInputs())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定要更改吗？", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            return;
        }
        else if (reply == QMessageBox::Yes)
        {
            if (DataBase::instance()->IsOpen())
            {
                QSqlQuery query = DataBase::instance()->Query();

                query.prepare(
                    "UPDATE "
                    "mouse "
                    "SET name = :name, description = :description , health = :health, master = :master "
                    "WHERE id = :id"
                );

                query.bindValue(":id", ui->edit_id->text().toInt());
                query.bindValue(":name", ui->edit_name->text().trimmed());
                query.bindValue(":description", ui->edit_description->toPlainText().trimmed());
                query.bindValue(":master", ui->box_master->currentText());
                query.bindValue(":health", ui->box_health->currentText());

                if (query.exec())
                {
                    if (query.isActive())
                    {
                        if (query.numRowsAffected() <= 0)
                        {
                            QMessageBox::warning(this, "注意", "没有信息被更改!");
                            qDebug() << query.lastError();
                            return;
                        }
                        RefreshMouse();
                        LoadMouse(ui->box_ID->currentText());
                        QMessageBox::information(this, "提示", "更新信息成功!");
                        qDebug() << "Database query OK." << query.lastQuery();
                        return;
                    }
                }
                else
                {
                    QMessageBox::warning(this, "注意", "更新信息失败!");
                    qDebug() << query.lastError();
                    return;
                }
            }
            else
            {
                QMessageBox::critical(this, "警告", "数据库连接断开!");
                qDebug() << "Database connection closed.";
                return;
            }
        }
    }
}

void Mouse::on_button_delete_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定要死亡吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }
    else if (reply == QMessageBox::Yes)
    {
        if (DataBase::instance()->IsOpen())
        {
            QSqlQuery query = DataBase::instance()->Query();

            query.prepare(
                "call mouse_dead(:id, :description) "
            );

            query.bindValue(":id", ui->edit_id->text().toInt());
            query.bindValue(":description", ui->edit_description->toPlainText().trimmed());

            if (query.exec())
            {
                if (query.isActive())
                {
                    if (query.numRowsAffected() <= 0)
                    {
                        QMessageBox::warning(this, "注意", "没有小鼠死亡!");
                        qDebug() << query.lastError();
                        return;
                    }
                    QMessageBox::information(this, "提示", "死亡记录成功!");
                    qDebug() << "Database query OK." << query.lastQuery();
                    qDebug() << "Delete Mouse: " << ui->box_ID->currentText();
                    ui->box_ID->removeItem(ui->box_ID->currentIndex());
                    Clear();
                    RefreshMouseDeath();
                    RefreshMouse();
                    return;
                }
            }
            else
            {
                QMessageBox::warning(this, "注意", "死亡失败!");
                qDebug() << query.lastError();
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, "警告", "数据库连接断开!");
            qDebug() << "Database connection closed.";
            return;
        }
    }
}

void Mouse::on_check_readonly_stateChanged(int arg1)
{
    ui->edit_id->setReadOnly(arg1);
    ui->edit_name->setReadOnly(arg1);
    ui->edit_description->setReadOnly(arg1);
    ui->box_health->setEnabled(!arg1);
    ui->box_master->setEnabled(!arg1);
}

void Mouse::on_box_ID_activated(const QString &arg1)
{
    LoadMouse(arg1);
}

void Mouse::EnableChange(bool arg1)
{
    ui->button_add->setEnabled(arg1);
    ui->button_delete->setEnabled(arg1);
    ui->button_update->setEnabled(arg1);
    if (arg1 == true)
        ui->check_readonly->setCheckState(Qt::Unchecked);
    else
        ui->check_readonly->setCheckState(Qt::Checked);
    ui->check_readonly->setEnabled(arg1);
}

void Mouse::on_btn_feed_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定要喂食吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }
    else if (reply == QMessageBox::Yes)
    {
        if (DataBase::instance()->IsOpen())
        {
            QSqlQuery query = DataBase::instance()->Query();

            query.prepare(
                "call feed_mouse(:id, :name) "
            );

            query.bindValue(":id", ui->edit_id->text().toInt());
            query.bindValue(":name", username);

            if (query.exec())
            {
                if (query.isActive())
                {
                    if (query.numRowsAffected() <= 0)
                    {
                        QMessageBox::warning(this, "注意", "没有选中小鼠!");
                        qDebug() << query.lastError();
                        return;
                    }
                    QMessageBox::information(this, "提示", "喂养成功!");
                    qDebug() << "Database query OK.";
                    RefreshMouse();
                    RefreshFeed();
                    return;
                }
            }
            else
            {
                QMessageBox::warning(this, "注意", "喂养失败!");
                qDebug() << query.lastError();
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, "警告", "数据库连接断开!");
            qDebug() << "Database connection closed.";
            return;
        }
    }
}
