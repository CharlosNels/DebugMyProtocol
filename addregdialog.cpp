#include "addregdialog.h"
#include "ui_addregdialog.h"
#include "openroutedialog.h"
#include "modbus_ascii.h"
#include "modbus_rtu.h"
#include "modbus_tcp.h"
#include "utils.h"

const QMap<QString,quint8> AddRegDialog::modbus_function_map = {
    {"01 Read Coils (0x)",ModbusReadCoils},
    {"02 Read Descrete Inputs (1x)", ModbusReadDescreteInputs},
    {"03 Read Holding Registers (4x)", ModbusReadHoldingRegisters},
    {"04 Read Input Registers (3x)", ModbusReadInputRegisters},
    {"05 Write Single Coil", ModbusWriteSingleCoil},
    {"06 Write Single Register", ModbusWriteSingleRegister},
    {"15 Write Multiple Coils", ModbusWriteMultipleCoils},
    {"16 Write Multiple Registers", ModbusWriteMultipleRegisters}
};

const QMap<QString,quint8> AddRegDialog::modbus_slave_function_map = {
    {"01 Coil Status (0x)", ModbusCoilStatus},
    {"02 Input Status (1x)", ModbusInputStatus},
    {"03 Holding Registers (4x)", ModbusHoldingRegisters},
    {"04 Input Registers (3x)", ModbusInputRegisters}
};

AddRegDialog::AddRegDialog(bool is_master, int protocol, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddRegDialog), m_protocol(protocol), m_is_master(is_master)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    if(is_master)
    {
        ui->box_function->addItems(modbus_function_map.keys());
    }
    else
    {
        ui->box_function->addItems(modbus_slave_function_map.keys());
        ui->box_scan_rate->hide();
        ui->label_scan_rate->hide();
        ui->label_scan_rate_unit->hide();
        ui->label_request->hide();
    }
    m_reg_def = nullptr;
}


AddRegDialog::AddRegDialog(bool is_master, int protocol, ModbusRegReadDefinitions *reg_def, QWidget *parent)
    :AddRegDialog(is_master, protocol, parent)
{
    m_reg_def = reg_def;
    ui->box_function->setCurrentText(getKeyByValue(is_master ? modbus_function_map:modbus_slave_function_map,reg_def->function));
    ui->box_id->setValue(reg_def->id);
    ui->box_addr->setValue(reg_def->reg_addr);
    ui->box_quantity->setValue(reg_def->quantity);
    if(is_master)
    {
        ui->box_scan_rate->setValue(reg_def->scan_rate);
    }
    if(is_master)
    {
        ui->label_request->setText(QString("%1 : %2").arg(tr("Request"), reg_def->packet.toHex(' ').toUpper()));
    }
    else
    {
        ui->label_request->clear();
    }
}

AddRegDialog::~AddRegDialog()
{
    delete ui;
}

void AddRegDialog::on_button_ok_clicked()
{
    ModbusRegReadDefinitions *def = new ModbusRegReadDefinitions;
    def->is_master = m_is_master;
    def->function = m_is_master ? modbus_function_map[ui->box_function->currentText()] : modbus_slave_function_map[ui->box_function->currentText()];
    def->id = ui->box_id->value();
    def->reg_addr = ui->box_addr->value();
    def->quantity = ui->box_quantity->value();
    def->scan_rate = ui->box_scan_rate->value();
    def->packet = m_request_bytes;
    if(m_reg_def)
    {
        emit readDefinitionsModified(def);
    }
    else
    {
        emit readDefinitionsCreated(def);
    }
    deleteLater();
}

void AddRegDialog::generateRequest()
{
    if(!m_is_master)
    {
        return;
    }
    ModbusFrameInfo frame_info;
    frame_info.id = ui->box_id->value();
    frame_info.function = modbus_function_map[ui->box_function->currentText()];
    frame_info.reg_addr = ui->box_addr->value();
    frame_info.quantity = ui->box_quantity->value();
    switch(m_protocol)
    {
    case MODBUS_RTU:
    {
        m_request_bytes = Modbus_RTU::masterFrame2Pack(frame_info);
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
    default:
    {
        break;
    }
    }
    ui->label_request->setText(QString("%1 : %2").arg(tr("Request"),m_request_bytes.toHex(' ').toUpper()));
}


void AddRegDialog::on_box_id_valueChanged(int arg1)
{
    generateRequest();
}


void AddRegDialog::on_box_function_currentTextChanged(const QString &arg1)
{
    if(arg1.contains("05") || arg1.contains("06"))
    {
        ui->box_quantity->setValue(1);
        ui->box_quantity->setEnabled(false);
    }
    else
    {
        ui->box_quantity->setEnabled(true);
    }
    generateRequest();
    
}


void AddRegDialog::on_box_addr_valueChanged(int arg1)
{
    generateRequest();
}


void AddRegDialog::on_box_quantity_valueChanged(int arg1)
{
    generateRequest();
}


void AddRegDialog::on_button_cancel_clicked()
{
    deleteLater();
}

