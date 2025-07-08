#ifndef WINDOWDISPLAY_H
#define WINDOWDISPLAY_H

#include <QMainWindow>
#include "stackmodel.h"
#include "registermodel.h"
#include "codemodel.h"
#include <machine/apple_iie.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class WindowDisplay;
}
QT_END_NAMESPACE

class WindowDisplay : public QMainWindow
{
    Q_OBJECT

public:
    WindowDisplay(QWidget *parent = nullptr);
    ~WindowDisplay();
    void setMachine(AppleIIe *);

private slots:
    void on_stepButton_released();

    void on_stepButton_pressed();

private:
    Ui::WindowDisplay *ui;
    StackModel *stack;
    RegisterModel *registers;
    CodeModel *code;
    AppleIIe *machine;

};
#endif // WINDOWDISPLAY_H
