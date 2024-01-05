#include "modbuswritemultiplecoilsdialog.h"
#include "ui_modbuswritemultiplecoilsdialog.h"
#include <QCheckBox>
#include <QListWidgetItem>
#include <QMessageBox>
#include "modbus_rtu.h"
#include "modbus_ascii.h"
#include "modbus_tcp.h"
#include "openroutedialog.h"
#include "modbuswidget.h"
#include "utils.h"

ModbusWriteMultipleCoilsDialog::ModbusWriteMultipleCoilsDialog(int protocol, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModbusWriteMultipleCoilsDialog), m_protocol(protocol)
{
    ui->setupUi(this);
    on_button_update_coil_list_clicked();
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("15:Write Multiple Coils"));
}

ModbusWriteMultipleCoilsDialog::~ModbusWriteMultipleCoilsDialog()
{
    delete ui;
}

void ModbusWriteMultipleCoilsDialog::responseSlot(int error_code)
{
    if(!isVisible())
    {
        return;
    }
    QString tip_string;
    if(error_code == ModbusErrorCode_OK)
    {
        tip_string = tr("Response ok");
    }
    else if(error_code == ModbusErrorCode_Timeout)
    {
        tip_string = tr("Timeout error");
    }
    else
    {
        tip_string = ModbusWidget::modbus_error_code_map[(ModbusErrorCode)error_code];
    }
    QMessageBox::warning(this, "Modbus", tip_string);
}

void ModbusWriteMultipleCoilsDialog::on_button_update_coil_list_clicked()
{
    int coils_quantity = ui->box_quantity->value();
    ui->list_coils->clear();
    for(auto x : m_coils_list)
    {
        x->deleteLater();
    }
    m_coils_list.clear();
    int coil_addr = ui->box_address->value();
    QCheckBox *coil_box{nullptr};
    QListWidgetItem *list_item{nullptr};
    for(int i = 0;i < coils_quantity;++i)
    {
        coil_box = new QCheckBox(QString("%1 %2").arg(tr("Coil")).arg(i + coil_addr), ui->list_coils);
        coil_box->setChecked(false);
        list_item = new QListWidgetItem(ui->list_coils);
        ui->list_coils->addItem(list_item);
        ui->list_coils->setItemWidget(list_item, coil_box);
        m_coils_list.append(coil_box);
    }
    ui->button_clear_all->setEnabled(true);
    ui->button_send->setEnabled(true);
    ui->button_set_all->setEnabled(true);
}


void ModbusWriteMultipleCoilsDialog::on_button_set_all_clicked()
{
    for(auto x:m_coils_list)
    {
        x->setChecked(true);
    }
}


void ModbusWriteMultipleCoilsDialog::on_button_clear_all_clicked()
{
    for(auto x:m_coils_list)
    {
        x->setChecked(true);
    }
}


void ModbusWriteMultipleCoilsDialog::on_button_send_clicked()
{
    ModbusFrameInfo frame_info{};
    frame_info.id = ui->box_slave_id->value();
    frame_info.function = ModbusWriteMultipleCoils;
    frame_info.reg_addr = ui->box_address->value();
    frame_info.quantity = ui->box_quantity->value();
    quint8 *coils = (quint8*)frame_info.reg_values;
    for(int i  = 0;i < frame_info.quantity;++i)
    {
        int byte_index = i / 16;
        int bit_index = i % 16;
        setBit(coils[byte_index],bit_index, m_coils_list[i]->isChecked());
    }
    QByteArray write_pack{};
    switch(m_protocol)
    {
    case MODBUS_RTU:
    {
        write_pack = Modbus_RTU::masterFrame2Pack(frame_info);
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
    emit writeFunctionTriggered(write_pack);
}


void ModbusWriteMultipleCoilsDialog::on_button_cancel_clicked()
{
    hide();
}


void ModbusWriteMultipleCoilsDialog::on_box_quantity_valueChanged(int arg1)
{
    ui->button_clear_all->setDisabled(true);
    ui->button_send->setDisabled(true);
    ui->button_set_all->setDisabled(true);
    for(auto x:m_coils_list)
    {
        x->setDisabled(true);
    }
}

