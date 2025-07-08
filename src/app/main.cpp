#include "windowdisplay.h"

#include <QApplication>
#include <spdlog/spdlog.h>

#include <apple_iie.hpp>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::info);

    AppleIIe *machine = new AppleIIe();

    machine->print();
    // machine->run();
    // for(int i = 0; i < 1000; i++) {
    //     machine->step();
    //     machine->print();
    // }

    // delete machine;

    QApplication a(argc, argv);
    WindowDisplay w;
    w.setMachine(machine);
    w.show();
    return a.exec();
}
