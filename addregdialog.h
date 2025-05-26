#ifndef ADDREGDIALOG_H
#define ADDREGDIALOG_H

#include <QDialog>
#include <QMap>
#include <QByteArray>
#include "ModbusBase.h"

namespace Ui {
class AddRegDialog;
}

struct ModbusRegReadDefinitions
{
    bool is_master{true};
    quint8 id;
    quint8 function;
    quint16 reg_addr;
    quint32 quantity;
    quint32 scan_rate;
    QByteArray packet;
};

class AddRegDialog : public QDialog
{
    Q_OBJECT

public:
    static const QMap<QString,quint8> modbus_function_map;
    static const QMap<QString,quint8> modbus_slave_function_map;
    explicit AddRegDialog(bool is_master, ModbusBase *modbus, QWidget *parent = nullptr);
    explicit AddRegDialog(bool is_master, ModbusBase *modbus, ModbusRegReadDefinitions *reg_def, QWidget *parent = nullptr);
    ~AddRegDialog();

signals:
    void readDefinitionsCreated(ModbusRegReadDefinitions *reg_def);
    void readDefinitionsModified(ModbusRegReadDefinitions *reg_def);

private slots:
    void on_button_ok_clicked();

    void on_box_id_valueChanged(int arg1);

    void on_box_function_currentTextChanged(const QString &arg1);

    void on_box_addr_valueChanged(int arg1);

    void on_box_quantity_valueChanged(int arg1);

    void on_button_cancel_clicked();

private:
    void generateRequest();

private:
    Ui::AddRegDialog *ui;
    ModbusRegReadDefinitions *m_reg_def;
    ModbusBase *m_modbus;
    QByteArray m_request_bytes;
    bool m_is_master;
};

#endif // ADDREGDIALOG_H
