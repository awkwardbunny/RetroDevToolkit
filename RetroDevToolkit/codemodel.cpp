#include "codemodel.h"
#include "galisting.h"
#include <iostream>

CodeModel::CodeModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    gas = new GoodASM("6502");
    // gas->setListing("nasm");
}

void CodeModel::load() {
    gas->baseaddress = 0;
    // gas->listadr = 0;
    // gas->listbytes = 0;
    // std::cout << std::format("{} {} {} {}", mach->ram[0], mach->ram[1], mach->ram[0xf800], mach->ram[0xf801]);
    gas->load(QByteArray::fromRawData((const char *)(mach->ram), 64*1024));
    update();
    // emit dataChanged(index(0, 0), index(64*1024, 1));
}

int CodeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return gas->instructions.length();
}

int CodeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant CodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QList<GAInstruction> instr = gas->instructions;

    if (role == Qt::DisplayRole) {
        if(index.column() == 0) {
            // return QString::fromStdString(std::format("{:04x}", index.row()));
            return QString::fromStdString(std::format("{:04x}", instr[index.row()].adr));
        } else {
            return QString::fromStdString(std::format("{} {}", instr[index.row()].verb.toStdString(), instr[index.row()].params.toStdString()));
        }
    }
    return QVariant();
}

bool CodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CodeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // FIXME: Implement me!
}

void CodeModel::update() {
    emit dataChanged(index(0, 0), index(gas->instructions.length(), 0));
}

void CodeModel::setMachine(AppleIIe *mach) {
    this->mach = mach;
}
