#ifndef MODBUSWRITESINGLECOILDIALOG_H
#define MODBUSWRITESINGLECOILDIALOG_H

#include <QDialog>
#include <QByteArray>

namespace Ui {
class ModbusWriteSingleCoilDialog;
}

class ModbusWriteSingleCoilDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusWriteSingleCoilDialog(int protocol, QWidget *parent = nullptr);
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
    int m_protocol;
};

#endif // MODBUSWRITESINGLECOILDIALOG_H
