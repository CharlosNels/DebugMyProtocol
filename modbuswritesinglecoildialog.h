#ifndef MODBUSWRITESINGLECOILDIALOG_H
#define MODBUSWRITESINGLECOILDIALOG_H

#include <QDialog>

namespace Ui {
class ModbusWriteSingleCoilDialog;
}

class ModbusBase;

class ModbusWriteSingleCoilDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusWriteSingleCoilDialog(ModbusBase *modbus, QWidget *parent = nullptr);
    ~ModbusWriteSingleCoilDialog();

signals:
    void writeFunctionTriggered(QByteArray pack);

public slots:
    void responseSlot(int error_code);

private slots:
    void on_button_send_clicked();

    void on_box_slave_id_valueChanged(int arg1);

    void on_box_address_valueChanged(int arg1);

    void on_button_on_toggled(bool checked);

    void on_button_05_function_toggled(bool checked);

    void on_button_cancel_clicked();

private:
    void generatePack();

private:
    Ui::ModbusWriteSingleCoilDialog *ui;
    QByteArray m_write_pack;
    ModbusBase *m_modbus;
};

#endif // MODBUSWRITESINGLECOILDIALOG_H
