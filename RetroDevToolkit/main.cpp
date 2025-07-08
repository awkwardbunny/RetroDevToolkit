#include "windowdisplay.h"

#include <QApplication>
#include <spdlog/spdlog.h>

#include <machine/apple_iie.hpp>
#include <common/ram.hpp>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    AppleIIe *machine = new AppleIIe();

    machine->print();
    // machine->run();
    // for(int i = 0; i < 1; i++) {
    //     machine->step();
    //     machine->print();
    // }

    delete machine;

    // QApplication a(argc, argv);
    // WindowDisplay w;
    // w.setMachine(machine);
    // w.show();
    // return a.exec();
}
