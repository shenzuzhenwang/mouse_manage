#ifndef Mouse_H
#define Mouse_H

#include <QDialog>
#include "DataBase.h"

namespace Ui
{
class Mouse;
}

class Mouse : public QDialog
{
    Q_OBJECT

public:
    explicit Mouse(QWidget *parent = 0);
    ~Mouse();
    QString username;

    void Clear();

    void LoadMouse(const QString id);

    void EnableChange(bool arg1);

signals:
    void RefreshMouse();
    void RefreshMouseDeath();
    void RefreshFeed();

private slots:
    void on_button_add_clicked();

    void on_button_update_clicked();

    void on_button_delete_clicked();

    void on_check_readonly_stateChanged(int arg1);

    void on_box_ID_activated(const QString &arg1);

    void on_btn_feed_clicked();

private:
    Ui::Mouse *ui;

    bool CheckInputs();

    void LoadToComboBox();
};

#endif // Mouse_H
