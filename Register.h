#ifndef Register_H
#define Register_H

#include <QDialog>
#include "DataBase.h"

//namespace UI里定义Regeister类
namespace Ui
{
class Register;
}

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();

    //只有类自己可以将信号与之相连接
private slots:
    void on_button_register_clicked();

private:
    Ui::Register *ui;

    void Clear();

    bool CheckInputs();
};

#endif // Register_H
