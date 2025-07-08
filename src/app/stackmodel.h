#ifndef STACKMODEL_H
#define STACKMODEL_H

#include <QAbstractTableModel>
#include <apple_iie.hpp>

class StackModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit StackModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setMachine(AppleIIe *);
    void update();

private:
    AppleIIe *mach;
};

#endif // STACKMODEL_H
