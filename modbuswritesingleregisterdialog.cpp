#include "modbuswritesingleregisterdialog.h"
#include "ui_modbuswritesingleregisterdialog.h"
#include "modbuswidget.h"
#include "ModbusBase.h"
#include "mapdefines.h"
#include <QMessageBox>

ModbusWriteSingleRegisterDialog::ModbusWriteSingleRegisterDialog(ModbusBase *modbus, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModbusWriteSingleRegisterDialog), m_modbus(modbus)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("06:Write Single Register"));
    generatePack();
}

ModbusWriteSingleRegisterDialog::~ModbusWriteSingleRegisterDialog()
{
    delete ui;
}

void ModbusWriteSingleRegisterDialog::responseSlot(int error_code)
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
            ui->label_result->setText(MapDefine.modbus_error_code_map[(ModbusErrorCode)error_code]);
        }
    }
}

void ModbusWriteSingleRegisterDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void ModbusWriteSingleRegisterDialog::on_button_cancel_clicked()
{
    hide();
}

void ModbusWriteSingleRegisterDialog::generatePack()
{
    ModbusFrameInfo frame_info{};
    frame_info.id = ui->box_slave_id->value();
    frame_info.reg_addr = ui->box_address->value();
    frame_info.reg_values[0] = ui->box_reg_value->value();
    frame_info.quantity = 1;
    frame_info.function = ui->button_06_function->isChecked() ? ModbusWriteSingleRegister : ModbusWriteMultipleRegisters;
    m_write_pack = m_modbus->masterFrame2Pack(frame_info);
    ui->label_request->setText(m_write_pack.toHex(' ').toUpper());
}


void ModbusWriteSingleRegisterDialog::on_button_send_clicked()
{
    emit writeFunctionTriggered(m_write_pack);
}


void ModbusWriteSingleRegisterDialog::on_box_slave_id_valueChanged(int arg1)
{
    generatePack();
}


void ModbusWriteSingleRegisterDialog::on_box_address_valueChanged(int arg1)
{
    generatePack();
}


void ModbusWriteSingleRegisterDialog::on_box_reg_value_valueChanged(int arg1)
{
    generatePack();
}


void ModbusWriteSingleRegisterDialog::on_button_06_function_toggled(bool checked)
{
    generatePack();
}

