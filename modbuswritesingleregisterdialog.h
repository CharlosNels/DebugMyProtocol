#ifndef MODBUSWRITESINGLEREGISTERDIALOG_H
#define MODBUSWRITESINGLEREGISTERDIALOG_H

#include <QDialog>
#include <QByteArray>

namespace Ui {
class ModbusWriteSingleRegisterDialog;
}

class ModbusWriteSingleRegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusWriteSingleRegisterDialog(int protocol, QWidget *parent = nullptr);
    ~ModbusWriteSingleRegisterDialog();

signals:
    void writeFunctionTriggered(QByteArray pack);

public slots:
    void responseSlot(int error_code);

private slots:
    void on_button_cancel_clicked();


    void on_button_send_clicked();

    void on_box_slave_id_valueChanged(int arg1);

    void on_box_address_valueChanged(int arg1);

    void on_box_reg_value_valueChanged(int arg1);

    void on_button_06_function_toggled(bool checked);

private:
    void generatePack();
private:
    Ui::ModbusWriteSingleRegisterDialog *ui;
    QByteArray m_write_pack;
    int m_protocol;
};

#endif // MODBUSWRITESINGLEREGISTERDIALOG_H
