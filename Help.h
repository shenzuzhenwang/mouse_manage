
#ifndef Help_H
#define Help_H

#include <QDialog>
#include <QTextBrowser>
#include <QLayout>

/**
 * @brief 帮助界面类
 */
class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

private:
    QTextBrowser* text;   // 显示的内容
    QGridLayout* layout;  // 大小框架
};

#endif // Help_H
