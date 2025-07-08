#ifndef WINDOWSTACK_H
#define WINDOWSTACK_H

#include <QDialog>

namespace Ui {
class WindowStack;
}

class WindowStack : public QDialog
{
    Q_OBJECT

public:
    explicit WindowStack(QWidget *parent = nullptr);
    ~WindowStack();

private:
    Ui::WindowStack *ui;
};

#endif // WINDOWSTACK_H
