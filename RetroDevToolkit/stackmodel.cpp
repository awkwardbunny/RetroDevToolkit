#include "stackmodel.h"

StackModel::StackModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int StackModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 256;
}

int StackModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant StackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if(index.column() == 1) return QString::fromStdString(std::format("{:04x}", 256+index.row()));
        else if(index.column() == 0) return QString((mach->getCpu()->reg_SP == index.row()) ? ">" : "");
        else {
            return QString::fromStdString(std::format("{:02x}", mach->ram[256+index.row()]));
        }
    }
    return QVariant();
}

bool StackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

void StackModel::update() {
    emit dataChanged(index(0, 0), index(255, 2));
}

void StackModel::setMachine(AppleIIe *mach) {
    this->mach = mach;
}

Qt::ItemFlags StackModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | (index.column() == 2 ? Qt::ItemIsEditable : Qt::ItemFlags()); // FIXME: Implement me!
}
