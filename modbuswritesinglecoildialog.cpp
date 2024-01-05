#include "modbuswritesinglecoildialog.h"
#include "ui_modbuswritesinglecoildialog.h"
#include "modbus_rtu.h"
#include "modbus_ascii.h"
#include "modbus_tcp.h"
#include "openroutedialog.h"
#include "modbuswidget.h"
#include <QMessageBox>

ModbusWriteSingleCoilDialog::ModbusWriteSingleCoilDialog(int protocol, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModbusWriteSingleCoilDialog), m_protocol(protocol)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("05:Write Single Coil"));
    generatePack();
}

ModbusWriteSingleCoilDialog::~ModbusWriteSingleCoilDialog()
{
    delete ui;
}

void ModbusWriteSingleCoilDialog::responseSlot(int error_code)
{
    if(!isVisible())
    {
        return;
    }
    if(error_code == ModbusErrorCode_OK)
    {
        ui->label_result->setText(tr("Response OK"));
        if(ui->box_close_on_ok->isChecked())
        {
            hide();
        }
    }
    else
    {
        if(error_code == ModbusErrorCode_Timeout)
        {
            ui->label_result->setText(tr("Timeout Error"));
        }
        else
        {
            ui->label_result->setText(ModbusWidget::modbus_error_code_map[(ModbusErrorCode)error_code]);
        }
    }
}

void ModbusWriteSingleCoilDialog::on_button_send_clicked()
{
    emit writeFunctionTriggered(m_write_pack);
}


void ModbusWriteSingleCoilDialog::on_box_slave_id_valueChanged(int arg1)
{
    generatePack();
}


void ModbusWriteSingleCoilDialog::on_box_address_valueChanged(int arg1)
{
    generatePack();
}


void ModbusWriteSingleCoilDialog::on_button_on_toggled(bool checked)
{
    generatePack();
}


void ModbusWriteSingleCoilDialog::on_button_05_function_toggled(bool checked)
{
    generatePack();
}

void ModbusWriteSingleCoilDialog::generatePack()
{
    ModbusFrameInfo frame_info{};
    frame_info.id = ui->box_slave_id->value();
    frame_info.reg_addr = ui->box_address->value();
    frame_info.quantity = 1;
    frame_info.reg_values[0] = ui->button_on->isChecked() ? (ui->button_05_function->isChecked() ? 0xFF00 : 0x01) : 0;
    frame_info.function = ui->button_05_function->isChecked() ? ModbusWriteSingleCoil : ModbusWriteMultipleCoils;
    switch (m_protocol) {
    case MODBUS_RTU:
    {
        m_write_pack = Modbus_RTU::masterFrame2Pack(frame_info);
        ui->label_request->setText(m_write_pack.toHex(' ').toUpper());
        break;
    }
    case MODBUS_ASCII:
    {

        break;
    }
    case MODBUS_TCP:
    {

        break;
    }
    case MODBUS_UDP:
    {

        break;
    }
    default:
        break;
    }
}


void ModbusWriteSingleCoilDialog::on_button_cancel_clicked()
{
    hide();
}

