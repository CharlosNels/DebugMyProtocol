#ifndef MODBUSWRITEMULTIPLEREGISTERSDIALOG_H
#define MODBUSWRITEMULTIPLEREGISTERSDIALOG_H

#include <QDialog>
#include "ModbusBase.h"

namespace Ui {
class ModbusWriteMultipleRegistersDialog;
}

class QListWidgetItem;

class ModbusWriteMultipleRegistersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusWriteMultipleRegistersDialog(ModbusBase *modbus, QWidget *parent = nullptr);
    ~ModbusWriteMultipleRegistersDialog();
signals:
    void writeFunctionTriggered(QByteArray pack);
public slots:
    void responseSlot(int error_code);

private slots:
    void on_box_quantity_valueChanged(int arg1);

    void on_button_update_value_list_clicked();

    void on_box_type_currentTextChanged(const QString &arg1);

    void on_list_values_itemDoubleClicked(QListWidgetItem *item);

    void on_button_cancel_clicked();

    void on_button_edit_clicked();

    void on_button_send_clicked();

private:
    bool update_value_list();

private:
    Ui::ModbusWriteMultipleRegistersDialog *ui;
    static const QMap<QString, int> format_map;
    ModbusBase *m_modbus;
    int m_format;
    quint16 *m_reg_values;
};

#endif // MODBUSWRITEMULTIPLEREGISTERSDIALOG_H
