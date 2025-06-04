#include "errorcounterdialog.h"
#include "ui_errorcounterdialog.h"
#include "modbuswidget.h"
#include "mapdefines.h"
#include <QListWidgetItem>

ErrorCounterDialog::ErrorCounterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ErrorCounterDialog)
{
    ui->setupUi(this);
}

ErrorCounterDialog::~ErrorCounterDialog()
{
    delete ui;
}

void ErrorCounterDialog::showEvent(QShowEvent *event)
{
    ui->list_errors->clear();
    QListWidgetItem *error_item{nullptr};
    for(auto x = m_error_count_map.constBegin();x != m_error_count_map.constEnd();++x)
    {
        error_item = new QListWidgetItem(QString("%1\t%2").arg(x.value()).arg(MapDefine.modbus_error_code_map[(ModbusErrorCode)x.key()]), ui->list_errors);
        error_item->setToolTip(MapDefine.modbus_error_code_comment_map[(ModbusErrorCode)x.key()]);
        ui->list_errors->addItem(error_item);
    }
}

void ErrorCounterDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void ErrorCounterDialog::increaseErrorCount(int error_code)
{
    if(m_error_count_map.contains(error_code))
    {
        m_error_count_map[error_code] = m_error_count_map[error_code] + 1;
    }
    else
    {
        m_error_count_map.insert(error_code, 1);
    }
}
