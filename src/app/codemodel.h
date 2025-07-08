#ifndef CODEMODEL_H
#define CODEMODEL_H

#include <QAbstractTableModel>
#include <apple_iie.hpp>
#include <goodasm.h>

class CodeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CodeModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void update();
    void setMachine(AppleIIe *);
    void load();

private:
    AppleIIe *mach;
    GoodASM *gas;
};

#endif // CODEMODEL_H
