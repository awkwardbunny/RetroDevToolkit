#include "registermodel.h"
#include <format>

RegisterModel::RegisterModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int RegisterModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 6;
}

int RegisterModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant RegisterModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        if(index.column() == 0) {
            switch(index.row()) {
            case 0:
                return QString("A");
            case 1:
                return QString("X");
            case 2:
                return QString("Y");
            case 3:
                return QString("PC");
            case 4:
                return QString("STACK");
            case 5:
                return QString("FLAGS");
            }
        } else {
            switch(index.row()) {
            case 0:
                return QString::fromStdString(std::format("{:02x}", cpu->reg_A));
            case 1:
                return QString::fromStdString(std::format("{:02x}", cpu->reg_IX));
            case 2:
                return QString::fromStdString(std::format("{:02x}", cpu->reg_IY));
            case 3:
                return QString::fromStdString(std::format("{:04x}", cpu->reg_PC));
            case 4:
                return QString::fromStdString(std::format("1{:02x}", cpu->reg_SP));
            case 5:
                return QString::fromStdString(std::format("{:02x}", cpu->reg_FLAGS));
            }

            return QString("");
        }
    else if(role == Qt::TextAlignmentRole) {
        if(index.column() == 0) return Qt::AlignRight;
        else return Qt::AlignLeft;
    }

    return QVariant();
}

bool RegisterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0) return false;

    QString x;
    bool status;
    uint8_t y;
    uint16_t z;
    if (data(index, role) != value) {
        // FIXME: Implement me!
        switch(index.row()) {
        case 0:
            y = value.toString().toUInt(&status, 16);
            cpu->reg_A= y;
            break;
        case 1:
            y = value.toString().toUInt(&status, 16);
            cpu->reg_IX= y;
            break;
        case 2:
            y = value.toString().toUInt(&status, 16);
            cpu->reg_IY= y;
            break;
        case 5:
            y = value.toString().toUInt(&status, 16);
            y &= 0xDF;
            cpu->reg_FLAGS = y;
            break;
        case 3:
            z = value.toString().toUInt(&status, 16);
            cpu->reg_PC = z;
            break;
        case 4:
            y = value.toString().toUInt(&status, 16);
            cpu->reg_SP = y;
            break;
        }

        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

void RegisterModel::update() {
    emit dataChanged(index(0, 1), index(5, 1));
}

Qt::ItemFlags RegisterModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | (index.column() == 1 ? Qt::ItemIsEditable : Qt::ItemFlags()); // FIXME: Implement me!
}

void RegisterModel::setCpu(MOS6502 *cpu) {
    this->cpu = cpu;
}
