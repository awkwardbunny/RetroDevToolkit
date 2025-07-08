#include "windowdisplay.h"
#include "ui_windowdisplay.h"
#include <QStandardItem>
#include <QStringListModel>

WindowDisplay::WindowDisplay(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WindowDisplay)
{
    ui->setupUi(this);

    stack = new StackModel();
    ui->stackView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->stackView->setFixedWidth(170);
    ui->stackView->setModel(stack);
    ui->stackView->horizontalHeader()->setVisible(false);
    ui->stackView->verticalHeader()->setVisible(false);
    ui->stackView->setColumnWidth(0, 20);
    ui->stackView->setColumnWidth(1, 45);
    ui->stackView->setColumnWidth(2, 45);

    registers = new RegisterModel();
    ui->registersView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    ui->registersView->setFixedWidth(170);
    ui->registersView->setModel(registers);
    ui->registersView->horizontalHeader()->setVisible(false);
    ui->registersView->verticalHeader()->setVisible(false);
    ui->registersView->setColumnWidth(0, 45);

    code = new CodeModel();
    ui->codeView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->codeView->setFixedWidth(200);
    ui->codeView->setModel(code);
    ui->codeView->horizontalHeader()->setVisible(false);
    ui->codeView->verticalHeader()->setVisible(false);
    ui->codeView->setColumnWidth(0, 45);

    ui->memoryView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->memoryView->setFixedWidth(200);
}

WindowDisplay::~WindowDisplay()
{
    delete ui;
}

void WindowDisplay::setMachine(AppleIIe *mach){
    machine = mach;
    registers->setCpu(mach->getCpu());
    stack->setMachine(mach);
    code->setMachine(mach);
    code->load();
}

void WindowDisplay::on_stepButton_released()
{
    registers->update();
    stack->update();
    code->update();
}


void WindowDisplay::on_stepButton_pressed()
{
    machine->step();
    machine->print();
}

