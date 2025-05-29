#ifndef MODBUSWRITEMULTIPLECOILSDIALOG_H
#define MODBUSWRITEMULTIPLECOILSDIALOG_H

#include <QDialog>

class QCheckBox;

namespace Ui {
class ModbusWriteMultipleCoilsDialog;
}

class ModbusBase;

class ModbusWriteMultipleCoilsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusWriteMultipleCoilsDialog(ModbusBase *modbus, QWidget *parent = nullptr);
    ~ModbusWriteMultipleCoilsDialog();
signals:
    void writeFunctionTriggered(QByteArray pack);
public slots:
    void responseSlot(int error_code);

private slots:
    void on_button_update_coil_list_clicked();

    void on_button_set_all_clicked();

    void on_button_clear_all_clicked();

    void on_button_send_clicked();

    void on_button_cancel_clicked();

    void on_box_quantity_valueChanged(int arg1);

    void on_button_reverse_all_clicked();

private:
    Ui::ModbusWriteMultipleCoilsDialog *ui;
    QList<QCheckBox*> m_coils_list;
    ModbusBase *m_modbus;
};

#endif // MODBUSWRITEMULTIPLECOILSDIALOG_H
