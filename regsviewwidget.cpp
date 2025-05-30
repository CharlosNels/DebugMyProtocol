#include "ui_regsviewwidget.h"
#include "addregdialog.h"
#include <QMenu>
#include <QActionGroup>
#include <QCursor>
#include <QInputDialog>
#include <QtEndian>
#include <QClipboard>
#include "regsviewwidget.h"
#include "utils.h"
#include "ModbusFrameInfo.h"
#include "ModbusRegReadDefinitions.h"

RegsViewWidget::RegsViewWidget(ModbusRegReadDefinitions *reg_def, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegsViewWidget), m_reg_defines(reg_def), m_send_count(0), m_error_count(0)
{
    ui->setupUi(this);
    QIcon nullIcon;
    setWindowIcon(nullIcon);
    m_table_model = new QStandardItemModel(ui->regs_table_view);
    m_table_model->setHorizontalHeaderLabels({tr("Register Address"),tr("Alias"),tr("Value")});
    m_table_model->setRowCount(reg_def->quantity);
    ui->regs_table_view->setModel(m_table_model);
    ui->regs_table_view->verticalHeader()->hide();
    ui->regs_table_view->setWordWrap(false);
    m_register_values = new quint16[reg_def->quantity]{0};
    for(quint32 i = 0;i < reg_def->quantity;++i)
    {
        m_table_model->setItem(i, 0, new QStandardItem());
        m_table_model->setItem(i, 1, new QStandardItem());
        m_table_model->setItem(i, 2, new QStandardItem());
        m_table_model->item(i, 0)->setText(QString::number(i + reg_def->reg_addr));
        m_table_model->item(i, 0)->setEditable(false);
        m_table_model->item(i, 1)->setEditable(true);
        m_table_model->item(i, 2)->setEditable(false);
        m_table_model->item(i,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table_model->item(i,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table_model->item(i,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if(reg_def->function == ModbusReadCoils || reg_def->function == ModbusReadDescreteInputs
        || reg_def->function == ModbusWriteMultipleCoils || reg_def->function == ModbusWriteSingleCoil)
        {
            m_cell_formats.append(Format_Coil);
        }
        else{
            m_cell_formats.append(Format_Signed);
        }
    }
    updateRegisterValues();
    const QString big_endian_str = tr("Big-endian");
    const QString little_endian_str = tr("Little_endian");
    const QString big_endian_byte_swap_str = tr("Big-endian byte swap");
    const QString little_endian_byte_swap_str = tr("Little_endian byte swap");
    m_popup_menu = new QMenu(ui->regs_table_view);
    m_popup_menu->hide();
    QMenu *format_menu = m_popup_menu->addMenu(tr("Format"));
    m_format_signed_action = format_menu->addAction(tr("Signed"));
    m_format_unsigned_action = format_menu->addAction(tr("Unsigned"));
    m_format_hex_action = format_menu->addAction(tr("Hex"));
    m_format_ascii_hex_action = format_menu->addAction(tr("ASCII - Hex"));
    m_format_binary_action = format_menu->addAction(tr("Binary"));
    format_menu->addSeparator();
    QMenu *format_32_bit_signed_menu = format_menu->addMenu(tr("32-bit Signed"));
    m_format_32_bit_signed_big_endian_action = format_32_bit_signed_menu->addAction(big_endian_str);
    m_format_32_bit_signed_little_endian_action = format_32_bit_signed_menu->addAction(little_endian_str);
    m_format_32_bit_signed_big_endian_byte_swap_action = format_32_bit_signed_menu->addAction(big_endian_byte_swap_str);
    m_format_32_bit_signed_little_endian_byte_swap_action = format_32_bit_signed_menu->addAction(little_endian_byte_swap_str);
    QMenu *format_32_bit_unsigned_menu = format_menu->addMenu(tr("32-bit Unsigned"));
    m_format_32_bit_unsigned_big_endian_action = format_32_bit_unsigned_menu->addAction(big_endian_str);
    m_format_32_bit_unsigned_little_endian_action = format_32_bit_unsigned_menu->addAction(little_endian_str);
    m_format_32_bit_unsigned_big_endian_byte_swap_action = format_32_bit_unsigned_menu->addAction(big_endian_byte_swap_str);
    m_format_32_bit_unsigned_little_endian_byte_swap_action = format_32_bit_unsigned_menu->addAction(little_endian_byte_swap_str);
    QMenu *format_64_bit_signed_menu = format_menu->addMenu(tr("64-bit Signed"));
    m_format_64_bit_signed_big_endian_action = format_64_bit_signed_menu->addAction(big_endian_str);
    m_format_64_bit_signed_little_endian_action = format_64_bit_signed_menu->addAction(little_endian_str);
    m_format_64_bit_signed_big_endian_byte_swap_action = format_64_bit_signed_menu->addAction(big_endian_byte_swap_str);
    m_format_64_bit_signed_little_endian_byte_swap_action = format_64_bit_signed_menu->addAction(little_endian_byte_swap_str);
    QMenu *format_64_bit_unsigned_menu = format_menu->addMenu(tr("64-bit Unsigned"));
    m_format_64_bit_unsigned_big_endian_action = format_64_bit_unsigned_menu->addAction(big_endian_str);
    m_format_64_bit_unsigned_little_endian_action = format_64_bit_unsigned_menu->addAction(little_endian_str);
    m_format_64_bit_unsigned_big_endian_byte_swap_action = format_64_bit_unsigned_menu->addAction(big_endian_byte_swap_str);
    m_format_64_bit_unsigned_little_endian_byte_swap_action = format_64_bit_unsigned_menu->addAction(little_endian_byte_swap_str);
    format_menu->addSeparator();
    QMenu *format_32_bit_float_menu = format_menu->addMenu(tr("32-bit Float"));
    m_format_32_bit_float_big_endian_action = format_32_bit_float_menu->addAction(big_endian_str);
    m_format_32_bit_float_little_endian_action = format_32_bit_float_menu->addAction(little_endian_str);
    m_format_32_bit_float_big_endian_byte_swap_action = format_32_bit_float_menu->addAction(big_endian_byte_swap_str);
    m_format_32_bit_float_little_endian_byte_swap_action = format_32_bit_float_menu->addAction(little_endian_byte_swap_str);
    QMenu *format_64_bit_float_menu = format_menu->addMenu(tr("64-bit Float"));
    m_format_64_bit_float_big_endian_action = format_64_bit_float_menu->addAction(big_endian_str);
    m_format_64_bit_float_little_endian_action = format_64_bit_float_menu->addAction(little_endian_str);
    m_format_64_bit_float_big_endian_byte_swap_action = format_64_bit_float_menu->addAction(big_endian_byte_swap_str);
    m_format_64_bit_float_little_endian_byte_swap_action = format_64_bit_float_menu->addAction(little_endian_byte_swap_str);
    m_popup_menu->addSeparator();
    m_copy_action = m_popup_menu->addAction(tr("Copy"));
    m_select_all_action = m_popup_menu->addAction(tr("Select All"));

    if(reg_def->is_master)
    {
        m_append_to_plot_graph_action = m_popup_menu->addAction(tr("Append to Plots"));
        connect(m_append_to_plot_graph_action, &QAction::triggered, this, &RegsViewWidget::appendToPlotsActionTriggered);
    }

    m_format_map = {
        {m_format_signed_action, Format_Signed},
        {m_format_unsigned_action, Format_Unsigned},
        {m_format_hex_action, Format_Hex},
        {m_format_ascii_hex_action, Format_Ascii_Hex},
        {m_format_binary_action, Format_Binary},
        {m_format_32_bit_signed_big_endian_action, Format_32_Bit_Signed_Big_Endian},
        {m_format_32_bit_signed_little_endian_action, Format_32_Bit_Signed_Little_Endian},
        {m_format_32_bit_signed_big_endian_byte_swap_action, Format_32_Bit_Signed_Big_Endian_Byte_Swap},
        {m_format_32_bit_signed_little_endian_byte_swap_action, Format_32_Bit_Signed_Little_Endian_Byte_Swap},
        {m_format_32_bit_unsigned_big_endian_action, Format_32_Bit_Unsigned_Big_Endian},
        {m_format_32_bit_unsigned_little_endian_action, Format_32_Bit_Unsigned_Little_Endian},
        {m_format_32_bit_unsigned_big_endian_byte_swap_action, Format_32_Bit_Unsigned_Big_Endian_Byte_Swap},
        {m_format_32_bit_unsigned_little_endian_byte_swap_action, Format_32_Bit_Unsigned_Little_Endian_Byte_Swap},
        {m_format_64_bit_signed_big_endian_action, Format_64_Bit_Signed_Big_Endian},
        {m_format_64_bit_signed_little_endian_action, Format_64_Bit_Signed_Little_Endian},
        {m_format_64_bit_signed_big_endian_byte_swap_action, Format_64_Bit_Signed_Big_Endian_Byte_Swap},
        {m_format_64_bit_signed_little_endian_byte_swap_action, Format_64_Bit_Signed_Little_Endian_Byte_Swap},
        {m_format_64_bit_unsigned_big_endian_action, Format_64_Bit_Unsigned_Big_Endian},
        {m_format_64_bit_unsigned_little_endian_action, Format_64_Bit_Unsigned_Little_Endian},
        {m_format_64_bit_unsigned_big_endian_byte_swap_action, Format_64_Bit_Unsigned_Big_Endian_Byte_Swap},
        {m_format_64_bit_unsigned_little_endian_byte_swap_action, Format_64_Bit_Unsigned_Little_Endian_Byte_Swap},
        {m_format_32_bit_float_big_endian_action, Format_32_Bit_Float_Big_Endian},
        {m_format_32_bit_float_little_endian_action, Format_32_Bit_Float_Little_Endian},
        {m_format_32_bit_float_big_endian_byte_swap_action, Format_32_Bit_Float_Big_Endian_Byte_Swap},
        {m_format_32_bit_float_little_endian_byte_swap_action, Format_32_Bit_Float_Little_Endian_Byte_Swap},
        {m_format_64_bit_float_big_endian_action, Format_64_Bit_Float_Big_Endian},
        {m_format_64_bit_float_little_endian_action, Format_64_Bit_Float_Little_Endian},
        {m_format_64_bit_float_big_endian_byte_swap_action, Format_64_Bit_Float_Big_Endian_Byte_Swap},
        {m_format_64_bit_float_little_endian_byte_swap_action, Format_64_Bit_Float_Little_Endian_Byte_Swap}
    };

    QActionGroup *format_actions_group = new QActionGroup(this);
    format_actions_group->setExclusive(true);
    QList<QAction*> format_actions{m_format_map.keys()};
    for(auto x : format_actions)
    {
        format_actions_group->addAction(x);
        x->setCheckable(true);
        connect(x, &QAction::triggered, this, &RegsViewWidget::formatActionTriggered);
        if(reg_def->function == ModbusReadCoils || reg_def->function == ModbusReadDescreteInputs)
        {
            x->setDisabled(true);
        }
    }

    m_format_signed_action->setChecked(true);
    connect(m_copy_action, &QAction::triggered, this, &RegsViewWidget::copyActionTriggered);
    connect(m_select_all_action, &QAction::triggered, this, &RegsViewWidget::selectAllActionTriggered);
    if(!m_reg_defines->is_master)
    {
        ui->info_label->setText(QString("ID=%1;F=%2").arg(m_reg_defines->id).arg(m_reg_defines->function,2,10,QChar('0')));
    }
}

RegsViewWidget::~RegsViewWidget()
{
    delete ui;
    delete []m_register_values;
}

void RegsViewWidget::setRegDef(ModbusRegReadDefinitions *reg_defines)
{
    m_reg_defines = reg_defines;
    m_table_model->clear();
    m_table_model->setHorizontalHeaderLabels({tr("Register Address"),tr("Alias"),tr("Value")});
    m_table_model->setRowCount(reg_defines->quantity);
    delete[] m_register_values;
    m_register_values = new quint16[reg_defines->quantity]{0};
    m_cell_formats.clear();
    for(quint32 i = 0;i < reg_defines->quantity;++i)
    {
        m_table_model->setItem(i, 0, new QStandardItem());
        m_table_model->setItem(i, 1, new QStandardItem());
        m_table_model->setItem(i, 2, new QStandardItem());
        m_table_model->item(i, 0)->setText(QString::number(i + reg_defines->reg_addr));
        m_table_model->item(i, 0)->setEditable(false);
        m_table_model->item(i, 1)->setEditable(true);
        m_table_model->item(i, 2)->setEditable(false);
        m_table_model->item(i,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table_model->item(i,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table_model->item(i,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if(reg_defines->function == ModbusReadCoils || reg_defines->function == ModbusReadDescreteInputs)
        {
            m_cell_formats.append(Format_Coil);
        }
        else{
            m_cell_formats.append(Format_Signed);
        }
    }
    updateRegisterValues();
    QList<QAction*> format_actions{m_format_map.keys()};
    for(auto x : format_actions)
    {
        if(reg_defines->function == ModbusReadCoils || reg_defines->function == ModbusReadDescreteInputs)
        {
            x->setDisabled(true);
        }
        else
        {
            x->setEnabled(true);
        }
    }
    if(!reg_defines->is_master)
    {
        ui->info_label->setText(QString("ID=%1;F=%2").arg(reg_defines->id).arg(reg_defines->function,2,10,QChar('0')));
    }
}

void RegsViewWidget::increaseErrorCount()
{
    m_error_count++;
    ui->info_label->setText(QString("Tx=%1;Err=%2;ID=%3;F=%4;SR=%5ms").arg(m_send_count).arg(m_error_count).arg(m_reg_defines->id).arg(m_reg_defines->function,2,10,QChar('0')).arg(m_reg_defines->scan_rate));
}

void RegsViewWidget::increaseSendCount()
{
    m_send_count++;
    ui->info_label->setText(QString("Tx=%1;Err=%2;ID=%3;F=%4;SR=%5ms").arg(m_send_count).arg(m_error_count).arg(m_reg_defines->id).arg(m_reg_defines->function,2,10,QChar('0')).arg(m_reg_defines->scan_rate));
}

void RegsViewWidget::setErrorInfo(const QString &error_info)
{
    ui->error_label->setText(error_info);
}

bool RegsViewWidget::setRegisterValues(const quint16 *reg_values, quint16 reg_addr, quint16 quantity)
{
    if(reg_addr >= m_reg_defines->reg_addr && reg_addr + quantity <= m_reg_defines->reg_addr + m_reg_defines->quantity)
    {
        quint16 index = reg_addr - m_reg_defines->reg_addr;
        for(int i = 0; i < quantity; ++i)
        {
            m_register_values[index + i] = reg_values[i];
        }
        updateRegisterValues();
    }
    else
    {
        return false;
    }
    return true;
}

bool RegsViewWidget::getRegisterValues(quint16 *reg_values, quint16 reg_addr, quint16 quantity) const
{
    if(reg_addr >= m_reg_defines->reg_addr && reg_addr + quantity <= m_reg_defines->reg_addr + m_reg_defines->quantity)
    {
        quint16 index = reg_addr - m_reg_defines->reg_addr;
        for(int i = 0; i < quantity; ++i)
        {
            reg_values[i] = m_register_values[index + i];
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool RegsViewWidget::setCoilValue(int coil_addr, quint16 value)
{
    if(coil_addr >= m_reg_defines->reg_addr && coil_addr < m_reg_defines->reg_addr + m_reg_defines->quantity)
    {
        m_register_values[coil_addr - m_reg_defines->reg_addr] = value;
        updateRegisterValues();
    }
    else
    {
        return false;
    }
    return true;
}

bool RegsViewWidget::getCoilValue(int coil_addr, quint16 *value) const
{
    if(coil_addr >= m_reg_defines->reg_addr && coil_addr < m_reg_defines->reg_addr + m_reg_defines->quantity)
    {
        *value = m_register_values[coil_addr - m_reg_defines->reg_addr];
    }
    else
    {
        return false;
    }
    return true;
}

void RegsViewWidget::formatActionTriggered()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    if(action)
    {
        QModelIndexList selections = ui->regs_table_view->selectionModel()->selectedIndexes();
        CellFormat format = m_format_map[action];
        if(format >= 32 && format < 64)
        {
            for(int row = selections.first().row();row <= selections.last().row();row += 2)
            {
                if(row > m_table_model->rowCount() - 2)
                {
                    break;
                }
                m_cell_formats[row] = format;
                m_cell_formats[row + 1] = CellFormat::Format_None;
            }
        }
        else if(format >= 64)
        {
            for(int row = selections.first().row();row <= selections.last().row();row += 4)
            {
                if(row > m_table_model->rowCount() - 4)
                {
                    break;
                }
                m_cell_formats[row] = format;
                m_cell_formats[row + 1] = CellFormat::Format_None;
                m_cell_formats[row + 2] = CellFormat::Format_None;
                m_cell_formats[row + 3] = CellFormat::Format_None;
            }
        }
        else
        {
            for(int row = selections.first().row();row <= selections.last().row();++row)
            {
                m_cell_formats[row] = format;
            }
        }
        updateRegisterValues();
    }
}

void RegsViewWidget::copyActionTriggered()
{
    QModelIndexList selections = ui->regs_table_view->selectionModel()->selectedIndexes();
    QString clip_text;
    for(int i = 0;i < selections.size(); ++i)
    {
        QModelIndex index = selections[i];
        if(index.isValid())
        {
            clip_text.append(m_table_model->item(index.row(), 2)->text() + '\n');
        }
    }
    if(clip_text.size() > 0)
    {
        QApplication::clipboard()->setText(clip_text);
    }
}

void RegsViewWidget::selectAllActionTriggered()
{
    ui->regs_table_view->selectColumn(2);
}

void RegsViewWidget::appendToPlotsActionTriggered()
{
    QModelIndexList selections = ui->regs_table_view->selectionModel()->selectedIndexes();
    for(int row = selections.first().row(); row <= selections.last().row(); ++row)
    {
        if(m_cell_formats[row] == Format_Signed || m_cell_formats[row] == Format_Unsigned || m_cell_formats[row] >= Format_32_Bit_Signed_Big_Endian)
        {
            register_value_t reg_val{&m_register_values[row], m_cell_formats[row], quint16(m_reg_defines->reg_addr + row), m_table_model->item(row, 1)->text()};
            emit append_plot_graph(reg_val);
        }
    }
}

void RegsViewWidget::updateRegisterValues()
{
    QString cell_text;
    for(quint32 i = 0;i < m_reg_defines->quantity; ++i)
    {
        switch(m_cell_formats[i])
        {
        case Format_None:
        {
            cell_text = "--";
            m_table_model->item(i, 2)->setText("--");
            break;
        }
        case Format_Coil:
        {
            cell_text = QString::number(m_register_values[i] ? 1 : 0);
            break;
        }
        case Format_Signed:
        {
            cell_text = QString::number(qint16(m_register_values[i]));
            break;
        }
        case Format_Unsigned:
        {
            cell_text = QString::number(quint16(m_register_values[i]));
            break;
        }
        case Format_Hex:
        {
            cell_text = QString("0x%1").arg(QString("%1").arg(quint16(m_register_values[i]), 4, 16, QChar('0')).toUpper());
            break;
        }
        case Format_Ascii_Hex:
        {
            cell_text = QString("(?%1) 0x%2").arg(QChar(char(m_register_values[i]))).arg(QString("%1").arg(quint16(m_register_values[i]), 4, 16, QChar('0').toUpper()));
            break;
        }
        case Format_Binary:
        {
            cell_text = QString("%1").arg(quint16(m_register_values[i]), 16, 2, QChar('0'));
            break;
        }
        case Format_32_Bit_Signed_Big_Endian:
        {
            qint32 value = myFromBigEndianByteSwap<qint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Signed_Little_Endian:
        {
            qint32 value = myFromLittleEndianByteSwap<qint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Signed_Big_Endian_Byte_Swap:
        {
            qint32 value = qFromBigEndian<qint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Signed_Little_Endian_Byte_Swap:
        {
            qint32 value = qFromLittleEndian<qint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Unsigned_Big_Endian:
        {
            quint32 value = myFromBigEndianByteSwap<quint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Unsigned_Little_Endian:
        {
            quint32 value = myFromLittleEndianByteSwap<quint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Unsigned_Big_Endian_Byte_Swap:
        {
            quint32 value = qFromBigEndian<quint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Unsigned_Little_Endian_Byte_Swap:
        {
            quint32 value = qFromLittleEndian<quint32>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Signed_Big_Endian:
        {
            qint64 value = myFromBigEndianByteSwap<qint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Signed_Little_Endian:
        {
            qint64 value = myFromLittleEndianByteSwap<qint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Signed_Big_Endian_Byte_Swap:
        {
            qint64 value = qFromBigEndian<qint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Signed_Little_Endian_Byte_Swap:
        {
            qint64 value = qFromLittleEndian<qint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Unsigned_Big_Endian:
        {
            quint64 value = myFromBigEndianByteSwap<quint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Unsigned_Little_Endian:
        {
            quint64 value = myFromLittleEndianByteSwap<quint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Unsigned_Big_Endian_Byte_Swap:
        {
            quint64 value = qFromBigEndian<quint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_64_Bit_Unsigned_Little_Endian_Byte_Swap:
        {
            quint64 value = qFromLittleEndian<quint64>(&m_register_values[i]);
            cell_text = QString::number(value);
            break;
        }
        case Format_32_Bit_Float_Big_Endian:
        {
            float fval = myFromBigEndianByteSwap<float>(&m_register_values[i]);
            cell_text = QString::number(fval);
            break;
        }
        case Format_32_Bit_Float_Little_Endian:
        {
            float fval = myFromLittleEndianByteSwap<float>(&m_register_values[i]);
            cell_text = QString::number(fval);
            break;
        }
        case Format_32_Bit_Float_Big_Endian_Byte_Swap:
        {
            float fval = qFromBigEndian<float>(&m_register_values[i]);
            cell_text = QString::number(fval);
            break;
        }
        case Format_32_Bit_Float_Little_Endian_Byte_Swap:
        {
            float fval = qFromLittleEndian<float>(&m_register_values[i]);
            cell_text = QString::number(fval);
            break;
        }
        case Format_64_Bit_Float_Big_Endian:
        {
            double dval = myFromBigEndianByteSwap<double>(&m_register_values[i]);
            cell_text = QString::number(dval);
            break;
        }
        case Format_64_Bit_Float_Little_Endian:
        {
            double dval = myFromLittleEndianByteSwap<double>(&m_register_values[i]);
            cell_text = QString::number(dval);
            break;
        }
        case Format_64_Bit_Float_Big_Endian_Byte_Swap:
        {
            double dval = qFromBigEndian<double>(&m_register_values[i]);
            cell_text = QString::number(dval);
            break;
        }
        case Format_64_Bit_Float_Little_Endian_Byte_Swap:
        {
            double dval = qFromLittleEndian<double>(&m_register_values[i]);
            cell_text = QString::number(dval);
            break;
        }
        default:
        {
            cell_text = "--";
            break;
        }
        }
        m_table_model->item(i, 2)->setText(cell_text);
    }
}

void RegsViewWidget::on_regs_table_view_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->regs_table_view->indexAt(pos);
    if(index.isValid())
    {
        QItemSelectionModel *selections = ui->regs_table_view->selectionModel();
        QModelIndexList selected_indexes = selections->selectedIndexes();
        bool is_same_format {true};
        CellFormat fmt = m_cell_formats[selected_indexes[0].row()];
        for(int i = 1; i < selected_indexes.size(); ++i)
        {
            if(fmt != m_cell_formats[selected_indexes[i].row()])
            {
                is_same_format = false;
                break;
            }
        }
        if(is_same_format)
        {
            QAction *action = getKeyByValue(m_format_map, fmt);
            if(action)
            {
                action->setChecked(true);
            }
            else
            {
                QList<QAction*> fmt_actions = m_format_map.keys();
                for(auto x : fmt_actions)
                {
                    x->setChecked(false);
                }
            }
        }
        else
        {
            QList<QAction*> fmt_actions = m_format_map.keys();
            for(auto x : fmt_actions)
            {
                x->setChecked(false);
            }
        }
        m_popup_menu->exec(QCursor::pos());
    }
    
}


void RegsViewWidget::on_regs_table_view_doubleClicked(const QModelIndex &index)
{
    if(index.isValid() && index.column() == 2)
    {   
        bool input_ok{false};
        bool cvt_ok{false};
        if(!m_reg_defines->is_master && m_reg_defines->function != ModbusWriteSingleCoil
            && m_reg_defines->function != ModbusWriteMultipleCoils
            && m_reg_defines->function != ModbusWriteSingleRegister
            && m_reg_defines->function != ModbusWriteMultipleRegisters)
        {
            switch(m_cell_formats[index.row()])
            {
            case Format_None:
            {
                return;
            }
            case Format_Coil:
            {
                quint16 *value = &m_register_values[index.row()];
                QString input_val = QInputDialog::getItem(this, tr("Edit Coil"), tr("Value:"), {tr("On"),tr("Off")}, *value ? 0 : 1, false, &input_ok);
                if(input_ok)
                {
                    m_register_values[index.row()] = input_val == tr("On") ? 1 : 0;
                }
                break;
            }
            case Format_Signed:
            {
                qint16 *value = (qint16*)&m_register_values[index.row()];
                qint16 input_val = QInputDialog::getInt(this, tr("Edit Register"), tr("Value:"), *value, -32768, 32767, 1, &input_ok);
                if(input_ok)
                {
                    *value = input_val;
                }
                break;
            }
            case Format_Unsigned:
            {
                quint16 *value = (quint16*)&m_register_values[index.row()];
                quint16 input_val = QInputDialog::getInt(this, tr("Edit Register"), tr("Value:"), *value, 0, 65535, 1, &input_ok);
                if(input_ok)
                {
                    *value = input_val;
                }
                break;
            }
            case Format_Hex:
            {
                quint16 *value = (quint16*)&m_register_values[index.row()];
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("Value:"), QLineEdit::Normal, QString::number(*value, 16), &input_ok);
                if(input_ok)
                {
                    quint16 new_val = input_val.toUInt(&cvt_ok, 16);
                    if(cvt_ok)
                    {
                        *value = new_val;
                    }
                }
                break;
            }
            case Format_Ascii_Hex:
            {
                return;
            }
            case Format_Binary:
            {
                return;
            }
            case Format_32_Bit_Signed_Big_Endian:
            {
                qint32 value = myFromBigEndianByteSwap<qint32>(&m_register_values[index.row()]);
                qint32 input_val = QInputDialog::getInt(this, tr("Edit Register"), tr("Value:"), value, -2147483648, 2147483647, 1, &input_ok);
                if(input_ok)
                {
                    myToBigEndianByteSwap<qint32>(input_val,&m_register_values[index.row()]);
                }
                break;
            }
            case Format_32_Bit_Signed_Little_Endian:
            {
                qint32 value = myFromLittleEndianByteSwap<qint32>(&m_register_values[index.row()]);
                qint32 input_val = QInputDialog::getInt(this, tr("Edit Register"), tr("Value:"), value, -2147483648, 2147483647, 1, &input_ok);
                if(input_ok)
                {
                    myToLittleEndianByteSwap<qint32>(input_val, &m_register_values[index.row()]);
                }
                break;
            }
            case Format_32_Bit_Signed_Big_Endian_Byte_Swap:
            {
                qint32 value = qFromBigEndian<qint32>(&m_register_values[index.row()]);
                qint32 input_val = QInputDialog::getInt(this, tr("Edit Register"), tr("value:"), value, -2147483648, 2147483647, 1, &input_ok);
                if(input_ok)
                {
                    qToBigEndian<qint32>(input_val, &m_register_values[index.row()]);
                }
                break;
            }
            case Format_32_Bit_Signed_Little_Endian_Byte_Swap:
            {
                qint32 value = qFromLittleEndian<qint32>(&m_register_values[index.row()]);
                qint32 input_val = QInputDialog::getInt(this, tr("Edit Register"), tr("value:"), value, -2147483648, 2147483647, 1, &input_ok);
                if(input_ok)
                {
                    qToLittleEndian<qint32>(input_val, &m_register_values[index.row()]);
                }
                break;
            }
            case Format_32_Bit_Unsigned_Big_Endian:
            {
                quint32 value = myFromBigEndianByteSwap<quint32>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint32 new_val = input_val.toUInt(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToBigEndianByteSwap<quint32>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Unsigned_Little_Endian:
            {
                quint32 value = myFromLittleEndianByteSwap<quint32>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint32 new_val = input_val.toUInt(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToLittleEndianByteSwap<quint32>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Unsigned_Big_Endian_Byte_Swap:
            {
                quint32 value = qFromBigEndian<quint32>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint32 new_val = input_val.toUInt(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToBigEndian<quint32>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Unsigned_Little_Endian_Byte_Swap:
            {
                quint32 value = qFromLittleEndian<quint32>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint32 new_val = input_val.toUInt(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToLittleEndian<quint32>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Signed_Big_Endian:
            {
                qint64 value = myFromBigEndianByteSwap<qint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    qint64 new_val = input_val.toLongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToBigEndianByteSwap<qint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Signed_Little_Endian:
            {
                qint64 value = myFromLittleEndianByteSwap<qint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    qint64 new_val = input_val.toLongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToLittleEndianByteSwap<qint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Signed_Big_Endian_Byte_Swap:
            {
                qint64 value = qFromBigEndian<qint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    qint64 new_val = input_val.toLongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToBigEndian<qint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Signed_Little_Endian_Byte_Swap:
            {
                qint64 value = qFromLittleEndian<qint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    qint64 new_val = input_val.toLongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToLittleEndian<qint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Unsigned_Big_Endian:
            {
                quint64 value = myFromBigEndianByteSwap<quint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint64 new_val = input_val.toULongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToBigEndianByteSwap<quint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Unsigned_Little_Endian:
            {
                quint64 value = myFromLittleEndianByteSwap<quint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint64 new_val = input_val.toULongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToLittleEndianByteSwap<quint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Unsigned_Big_Endian_Byte_Swap:
            {
                quint64 value = qFromBigEndian<quint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint64 new_val = input_val.toULongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToBigEndian<quint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Unsigned_Little_Endian_Byte_Swap:
            {
                quint64 value = qFromLittleEndian<quint64>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    quint64 new_val = input_val.toULongLong(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToLittleEndian<quint64>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Float_Big_Endian:
            {
                float value = myFromBigEndianByteSwap<float>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    float new_val = input_val.toFloat(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToBigEndianByteSwap<float>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Float_Little_Endian:
            {
                float value = myFromLittleEndianByteSwap<float>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    float new_val = input_val.toFloat(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToLittleEndianByteSwap<float>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Float_Big_Endian_Byte_Swap:
            {
                float value = qFromBigEndian<float>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    float new_val = input_val.toFloat(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToBigEndian<float>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_32_Bit_Float_Little_Endian_Byte_Swap:
            {
                float value = qFromLittleEndian<float>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    float new_val = input_val.toFloat(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToLittleEndian<float>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Float_Big_Endian:
            {
                double value = myFromBigEndianByteSwap<double>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    double new_val = input_val.toDouble(&cvt_ok);
                    if(cvt_ok)
                    {
                        myToBigEndianByteSwap<double>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Float_Little_Endian:
            {
                double value = myFromLittleEndianByteSwap<double>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    double new_val = input_val.toDouble(&cvt_ok);
                    if(input_ok)
                    {
                        myToLittleEndianByteSwap<double>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Float_Big_Endian_Byte_Swap:
            {
                double value = qFromBigEndian<double>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    double new_val = input_val.toDouble(&cvt_ok);
                    if(input_ok)
                    {
                        qToBigEndian<double>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            case Format_64_Bit_Float_Little_Endian_Byte_Swap:
            {
                double value = qFromLittleEndian<double>(&m_register_values[index.row()]);
                QString input_val = QInputDialog::getText(this, tr("Edit Register"), tr("value:"), QLineEdit::Normal, QString("%1").arg(value), &input_ok);
                if(input_ok)
                {
                    double new_val = input_val.toDouble(&cvt_ok);
                    if(cvt_ok)
                    {
                        qToLittleEndian<double>(new_val, &m_register_values[index.row()]);
                    }
                }
                break;
            }
            default:
            {
                return;
            }
            }
            updateRegisterValues();
        }
        else if(m_reg_defines->is_master
                &&(m_reg_defines->function == ModbusReadCoils || m_reg_defines->function == ModbusReadHoldingRegisters))
        {
            bool data_valid{false};
            ModbusFrameInfo frame_info{};
            frame_info.id = m_reg_defines->id;
            frame_info.reg_addr = m_reg_defines->reg_addr + index.row();
            switch(m_cell_formats[index.row()])
            {
                case Format_Coil:
                {
                    int item_index = m_register_values[index.row()] ? 0 : 1;
                    QString input_item = QInputDialog::getItem(this, tr("Edit Coil Value"), tr("Value:"), {tr("On"),tr("Off")}, item_index, false, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteSingleCoil;
                        frame_info.quantity = 1;
                        frame_info.reg_values[0] = input_item == tr("On") ? 0xFF00 : 0;
                    }
                    break;
                }
                case Format_Signed:
                {
                    qint16 *value = (qint16*)&m_register_values[index.row()];
                    qint16 input_value = QInputDialog::getInt(this, tr("Edit Signed Value"), tr("Value:"), *value, -32768, 32767, 1, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteSingleRegister;
                        frame_info.quantity = 1;
                        qint16 *reg_val = (qint16*)&frame_info.reg_values[0];
                        *reg_val = input_value;
                    }
                    break;
                }
                case Format_Unsigned:
                {
                    quint16 *value = (quint16*)&m_register_values[index.row()];
                    quint16 input_value = QInputDialog::getInt(this, tr("Edit Unsigned Value"), tr("Value:"), *value, 0, 65535, 1, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteSingleRegister;
                        frame_info.quantity = 1;
                        quint16 *reg_val = (quint16*)&frame_info.reg_values[0];
                        *reg_val = input_value;
                    }
                    break;
                }
                case Format_Ascii_Hex:
                case Format_Hex:
                {
                    QString input_value = QInputDialog::getText(this, tr("Edit Hex Value"), tr("Value:"), QLineEdit::Normal, QString::number(*(quint16*)&m_register_values[index.row()], 16), &input_ok);
                    if(input_ok)
                    {
                        quint16 value = input_value.toUShort(&cvt_ok, 16);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteSingleRegister;
                            frame_info.quantity = 1;
                            quint16 *reg_val = (quint16*)&frame_info.reg_values[0];
                            *reg_val = value;
                        }
                    }
                    break;
                }
                case Format_Binary:
                {
                    QString input_value = QInputDialog::getText(this, tr("Edit Binary Value"), tr("Value:"), QLineEdit::Normal, QString::number(*(quint16*)&m_register_values[index.row()], 2), &input_ok);
                    if(input_ok)
                    {
                        quint16 value = input_value.toUShort(&cvt_ok, 2);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteSingleRegister;
                            frame_info.quantity = 1;
                            quint16 *reg_val = (quint16*)&frame_info.reg_values[0];
                            *reg_val = value;
                        }
                    }
                    break;
                }
                case Format_32_Bit_Signed_Big_Endian:
                {
                    qint32 value = myFromBigEndianByteSwap<qint32>(&m_register_values[index.row()]);
                    qint32 input_value = QInputDialog::getInt(this, tr("Edit 32-Bit Signed Value"), tr("Value:"), value, -2147483648, 2147483647, 1, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteMultipleRegisters;
                        frame_info.quantity = 2;
                        myToBigEndianByteSwap<qint32>(input_value, &frame_info.reg_values[0]);
                    }
                    break;
                }
                case Format_32_Bit_Signed_Little_Endian:
                {
                    qint32 value = myFromLittleEndianByteSwap<qint32>(&m_register_values[index.row()]);
                    qint32 input_value = QInputDialog::getInt(this, tr("Edit 32-Bit Signed Value"), tr("Value:"), value, -2147483648, 2147483647, 1, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteMultipleRegisters;
                        frame_info.quantity = 2;
                        myToLittleEndianByteSwap<qint32>(input_value, &frame_info.reg_values[0]);
                    }
                    break;
                }
                case Format_32_Bit_Signed_Big_Endian_Byte_Swap:
                {
                    qint32 value = qFromBigEndian<qint32>(&m_register_values[index.row()]);
                    qint32 input_value = QInputDialog::getInt(this, tr("Edit 32-Bit Signed Value"), tr("Value:"), value, -2147483648, 2147483647, 1, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteMultipleRegisters;
                        frame_info.quantity = 2;
                        qToBigEndian<qint32>(input_value, &frame_info.reg_values[0]);
                    }
                    break;
                }
                case Format_32_Bit_Signed_Little_Endian_Byte_Swap:
                {
                    qint32 value = qFromLittleEndian<qint32>(&m_register_values[index.row()]);
                    qint32 input_value = QInputDialog::getInt(this, tr("Edit 32-Bit Signed Value"), tr("Value:"), value, -2147483648, 2147483647, 1, &input_ok);
                    if(input_ok)
                    {
                        data_valid = true;
                        frame_info.function = ModbusWriteMultipleRegisters;
                        frame_info.quantity = 2;
                        qToLittleEndian<qint32>(input_value, &frame_info.reg_values[0]);
                    }
                    break;
                }
                case Format_32_Bit_Unsigned_Big_Endian:
                {
                    quint32 value = myFromBigEndianByteSwap<quint32>(&m_register_values[index.row()]);
                    QString input_value = QInputDialog::getText(this, tr("Edit 32-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint32 new_val = input_value.toUInt(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            myToBigEndianByteSwap<quint32>(new_val, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Unsigned_Little_Endian:
                {
                    quint32 value = myFromLittleEndianByteSwap<quint32>(&m_register_values[index.row()]);
                    QString input_value = QInputDialog::getText(this, tr("Edit 32-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint32 new_val = input_value.toUInt(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            myToLittleEndianByteSwap<quint32>(new_val, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Unsigned_Big_Endian_Byte_Swap:
                {
                    quint32 value = qFromBigEndian<quint32>(&m_register_values[index.row()]);
                    QString input_value = QInputDialog::getText(this, tr("Edit 32-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint32 new_val = input_value.toUInt(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            qToBigEndian<quint32>(new_val, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Unsigned_Little_Endian_Byte_Swap:
                {
                    quint32 value = qFromLittleEndian<quint32>(&m_register_values[index.row()]);
                    QString input_value = QInputDialog::getText(this, tr("Edit 32-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint32 new_val = input_value.toUInt(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            qToLittleEndian<quint32>(new_val, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Signed_Big_Endian:
                {
                    qint64 value = myFromBigEndianByteSwap<qint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Signed Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        qint64 input_value = input_text.toLongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            myToBigEndianByteSwap<qint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Signed_Little_Endian:
                {
                    qint64 value = myFromLittleEndianByteSwap<qint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Signed Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        qint64 input_value = input_text.toLongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            myToLittleEndianByteSwap<qint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Signed_Big_Endian_Byte_Swap:
                {
                    qint64 value = qFromBigEndian<qint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Signed Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        qint64 input_value = input_text.toLongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            qToBigEndian<qint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Signed_Little_Endian_Byte_Swap:
                {
                    qint64 value = qFromLittleEndian<qint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Signed Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        qint64 input_value = input_text.toLongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            qToLittleEndian<qint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Unsigned_Big_Endian:
                {
                    quint64 value = myFromBigEndianByteSwap<quint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint64 input_value = input_text.toULongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            myToBigEndianByteSwap<quint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Unsigned_Little_Endian:
                {
                    quint64 value = myFromLittleEndianByteSwap<quint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint64 input_value = input_text.toULongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            myToLittleEndianByteSwap<quint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Unsigned_Big_Endian_Byte_Swap:
                {
                    quint64 value = qFromBigEndian<quint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint64 input_value = input_text.toULongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            qToBigEndian<quint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Unsigned_Little_Endian_Byte_Swap:
                {
                    quint64 value = qFromLittleEndian<quint64>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Unsigned Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        quint64 input_value = input_text.toULongLong(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            qToLittleEndian<quint64>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Float_Big_Endian:
                {
                    float value = myFromBigEndianByteSwap<float>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 32-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        float input_value = input_text.toFloat(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            myToBigEndianByteSwap<float>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Float_Little_Endian:
                {
                    float value = myFromLittleEndianByteSwap<float>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 32-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        float input_value = input_text.toFloat(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            myToLittleEndianByteSwap<float>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Float_Big_Endian_Byte_Swap:
                {
                    float value = qFromBigEndian<float>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 32-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        float input_value = input_text.toFloat(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            qToBigEndian<float>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_32_Bit_Float_Little_Endian_Byte_Swap:
                {
                    float value = qFromLittleEndian<float>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 32-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        float input_value = input_text.toFloat(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 2;
                            qToLittleEndian<float>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Float_Big_Endian:
                {
                    double value = myFromBigEndianByteSwap<double>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        double input_value = input_text.toDouble(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            myToBigEndianByteSwap<double>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Float_Little_Endian:
                {
                    double value = myFromLittleEndianByteSwap<double>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        double input_value = input_text.toDouble(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            myToLittleEndianByteSwap<double>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Float_Big_Endian_Byte_Swap:
                {
                    double value = qFromBigEndian<double>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        double input_value = input_text.toDouble(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            qToBigEndian<double>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                case Format_64_Bit_Float_Little_Endian_Byte_Swap:
                {
                    double value = qFromLittleEndian<double>(&m_register_values[index.row()]);
                    QString input_text = QInputDialog::getText(this, tr("Edit 64-Bit Float Value"), tr("Value:"), QLineEdit::Normal, QString::number(value), &input_ok);
                    if(input_ok)
                    {
                        double input_value = input_text.toDouble(&cvt_ok);
                        if(cvt_ok)
                        {
                            data_valid = true;
                            frame_info.function = ModbusWriteMultipleRegisters;
                            frame_info.quantity = 4;
                            qToLittleEndian<double>(input_value, &frame_info.reg_values[0]);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            if(data_valid)
            {
                emit writeFunctionTriggered(frame_info);
            }
        }
    }
}

void RegsViewWidget::closeEvent(QCloseEvent *event)
{
    emit closed(m_reg_defines);
    deleteLater();
}

