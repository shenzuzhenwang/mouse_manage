/********************************************************************************
** 帮助界面类
** 用于显示帮助界面
********************************************************************************/
#ifndef Help_H
#define Help_H

#include <QDialog>
#include <QTextBrowser>
#include <QLayout>

class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

private:
    QTextBrowser* text;
    QGridLayout* layout;
};

#endif // Help_H
