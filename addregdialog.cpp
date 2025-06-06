#include "addregdialog.h"
#include "ui_addregdialog.h"
#include "ModbusBase.h"
#include "utils.h"
#include "ModbusRegReadDefinitions.h"
#include "ModbusFrameInfo.h"
#include "mapdefines.h"


AddRegDialog::AddRegDialog(bool is_master, ModbusBase *modbus, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddRegDialog), m_modbus(modbus), m_is_master(is_master)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    if(is_master)
    {
        ui->box_function->addItems(MapDefine.modbus_function_map.keys());
        ui->box_quantity->setMaximum(125);
    }
    else
    {
        ui->box_function->addItems(MapDefine.modbus_slave_function_map.keys());
        ui->box_scan_rate->hide();
        ui->label_scan_rate->hide();
        ui->label_scan_rate_unit->hide();
        ui->label_request->hide();
        ui->box_quantity->setMaximum(10000);
    }
    m_reg_def = nullptr;
}


AddRegDialog::AddRegDialog(bool is_master, ModbusBase *modbus, ModbusRegReadDefinitions *reg_def, QWidget *parent)
    :AddRegDialog(is_master, modbus, parent)
{
    m_reg_def = reg_def;
    ui->box_function->setCurrentText(getKeyByValue(is_master ? MapDefine.modbus_function_map:MapDefine.modbus_slave_function_map,reg_def->function));
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
    def->function = m_is_master ? MapDefine.modbus_function_map[ui->box_function->currentText()] : MapDefine.modbus_slave_function_map[ui->box_function->currentText()];
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
    frame_info.function = MapDefine.modbus_function_map[ui->box_function->currentText()];
    frame_info.reg_addr = ui->box_addr->value();
    frame_info.quantity = ui->box_quantity->value();
    m_request_bytes = m_modbus->masterFrame2Pack(frame_info);
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

void AddRegDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

