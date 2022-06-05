/*********************************************************************
 * @file   Main.cpp
 * @brief 小鼠详细信息与功能界面，显示ID号、名称、描述、健康程度、负责人、出生日期和死亡日期，
 *        并且具有喂食、出生、死亡、修改信息等功能
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
#include "Mouse.h"
#include "ui_Mouse.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

 /*
  * @brief 小鼠详细信息与功能界面初始化函数，加载选中小鼠的信息
  */
Mouse::Mouse(QWidget *parent) : QDialog(parent), ui(new Ui::Mouse)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // 窗口大小固定

    Clear();  // 清除上次的信息

    LoadToComboBox();
    ui->time_birth->setDateTime(QDateTime::currentDateTime());
    ui->time_lastmeal->setDateTime(QDateTime::currentDateTime());
}

/*
 * @brief 析构界面
 */
Mouse::~Mouse()
{
    delete ui;
}

/*
 * @brief 清除界面中的各项信息
 */
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

/*
 * @brief 查看输入的ID格式
 */
bool Mouse::CheckInputs()
{
    if (ui->edit_id->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "注意", "id不能为空!");
        return false;
    }
    return true;
}

/*
 * @brief 连接数据库，将小鼠的各项信息，加入到ComboBox成员中
 */
void Mouse::LoadToComboBox()
{
    // 执行一个sql语句"SELECT id FROM mouse ORDER BY id"，找到所有小鼠的id
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

    // 执行一个sql语句"SELECT name FROM user"，找到所有主人的名字
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

/*
 * @brief 连接数据库，通过小鼠的id，在数据库中查找小鼠的信息，并加载到界面
 * @param id 带查询小鼠的id
 */
void Mouse::LoadMouse(const QString id)
{
    // 执行一个sql语句"SELECT * FROM mouse WHERE id = :id"，查找小鼠的信息
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

/*
 * @brief 小鼠出生功能，根据所选的信息，生成新的小鼠，并加入数据库中
 */
void Mouse::on_button_add_clicked()
{
    if (CheckInputs())
    {
        // 询问"确定出生吗？"
        QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定出生吗？", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            return;
        }
        else if (reply == QMessageBox::Yes)  // 确定出生
        {
            if (DataBase::instance()->IsOpen())
            {
                // 执行一个sql语句"select mouse_birth (:id, :name, :master, :health, :description)"，调用数据库sql函数
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
                        if (query.value(0) == 1)  // sql函数mouse_birth调用成功
                        {
                            QMessageBox::information(this, "提示", "出生记录成功!");
                            qDebug() << "Database query OK." << query.lastQuery();
                            ui->box_ID->insertItem(ui->box_ID->count() - 1, ui->edit_id->text().trimmed());
                            Clear();
                            RefreshMouse();
                            return;
                        }
                        else          // sql函数mouse_birth调用失败，主键id重复
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

/*
 * @brief 小鼠更改功能，根据所选的信息，更改指定id小鼠的信息，并上传数据库
 */
void Mouse::on_button_update_clicked()
{
    // 确定一个被更改的id的小鼠
    if (QString::compare(ui->edit_id->text().trimmed(), ui->box_ID->currentText(), Qt::CaseSensitive) != 0)
    {
        QMessageBox::warning(this, "注意", "更新的id应相同!");
        ui->edit_id->setText(ui->box_ID->currentText());
        return;
    }

    if (CheckInputs())
    {
        // 询问"确定要更改吗？"
        QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定要更改吗？", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            return;
        }
        else if (reply == QMessageBox::Yes)  // 确定更改
        {
            if (DataBase::instance()->IsOpen())
            {
                // 执行一个sql语句"UPDATE mouse SET name = :name, description = :description , health = :health, master = :master WHERE id = :id"，调用数据库sql函数
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
                        if (query.numRowsAffected() <= 0)  // 小鼠id不存在
                        {
                            QMessageBox::warning(this, "注意", "没有信息被更改!");
                            qDebug() << query.lastError();
                            return;
                        }
                        RefreshMouse();       // 信息更改成功
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

/*
 * @brief 小鼠死亡功能，根据所选的信息，死亡指定id小鼠，并上传数据库
 */
void Mouse::on_button_delete_clicked()
{
    // 询问"确定要死亡吗？"
    QMessageBox::StandardButton reply = QMessageBox::question(this, "请问", "确定要死亡吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }
    else if (reply == QMessageBox::Yes)  // 确定死亡
    {
        if (DataBase::instance()->IsOpen())
        {
            // 执行一个sql语句"call mouse_dead(:id, :description)"，调用数据库sql函数
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
                    if (query.numRowsAffected() <= 0)  // 小鼠id不存在
                    {
                        QMessageBox::warning(this, "注意", "没有小鼠死亡!");
                        qDebug() << query.lastError();
                        return;
                    }
                    QMessageBox::information(this, "提示", "死亡记录成功!");     // 死亡函数执行成功
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

/*
 * @brief 切换至只读模式
 * @param arg1 true 只读；false 可写
 */
void Mouse::on_check_readonly_stateChanged(int arg1)
{
    ui->edit_id->setReadOnly(arg1);
    ui->edit_name->setReadOnly(arg1);
    ui->edit_description->setReadOnly(arg1);
    ui->box_health->setEnabled(!arg1);
    ui->box_master->setEnabled(!arg1);
}

/*
 * @brief 根据选中的id加载小鼠的信息
 * @param arg1 选中的id
 */
void Mouse::on_box_ID_activated(const QString &arg1)
{
    LoadMouse(arg1);
}

/*
 * @brief 小鼠的信息是否可更改
 * @param arg1 true 可更改；fales 不可更改
 */
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

/*
 * @brief 小鼠喂食功能，根据所选的信息，喂食指定id小鼠，并上传数据库
 */
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
            // 执行一个sql语句"call feed_mouse(:id, :name)"，调用数据库sql函数
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
                    if (query.numRowsAffected() <= 0)  // 小鼠id不存在
                    {
                        QMessageBox::warning(this, "注意", "没有选中小鼠!");
                        qDebug() << query.lastError();
                        return;
                    }
                    QMessageBox::information(this, "提示", "喂养成功!");  // 喂养成功
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
