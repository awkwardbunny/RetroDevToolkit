#include "windowstack.h"
#include "ui_windowstack.h"

WindowStack::WindowStack(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WindowStack)
{
    ui->setupUi(this);
    setWindowTitle("[*] Stack");
    ui->tableWidget->setColumnCount(3);

}

WindowStack::~WindowStack()
{
    delete ui;
}
