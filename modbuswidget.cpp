#include "modbuswidget.h"
#include "modbuswritesinglecoildialog.h"
#include "ui_modbuswidget.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFrame>
#include <QTimer>
#include <QInputDialog>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QIcon>
#include <QIODevice>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QMainWindow>
#include "ModbusRegReadDefinitions.h"
#include "addregdialog.h"
#include "displaycommunication.h"
#include "floatbox.h"
#include "regsviewwidget.h"
#include "utils.h"
#include "errorcounterdialog.h"
#include "modbuswritemultiplecoilsdialog.h"
#include "modbuswritemultipleregistersdialog.h"
#include "modbuswritesinglecoildialog.h"
#include "modbuswritesingleregisterdialog.h"
#include "ModbusBase.h"
#include "plotwindow.h"
#include "testcenterwindow.h"

#define PRINT_TRAFFIC 0
#define CYCLE_QUEUE_MAX_SIZE 128

#define REG_DEF_IS_MASTER_STR "Is Master"
#define REG_DEF_ID_STR "ID"
#define REG_DEF_FUNCTION_STR "Function"
#define REG_DEF_ADDR_STR "Register Address"
#define REG_DEF_QUANTITY_STR "Quantity"
#define REG_DEF_SR_STR "Scan Rate"
#define REG_DEF_PACK_STR "Packet"
#define REG_DEF_REG_INFO_STR "Register Info"
#define REG_DEF_ADDR_OFFSET_STR "Register Address Offset"
#define REG_DEF_ADDR_ALIAS_STR "Register Alias"
#define REG_DEF_ADDR_VALUE_STR "Register Value"
#define REG_DEF_ADDR_FORMAT_STR "Register Format"

const QMap<ModbusErrorCode, QString> ModbusWidget::modbus_error_code_map = {
    {ModbusErrorCode_Timeout, tr("Timeout Error")},
    {ModbusErrorCode_Illegal_Function, tr("Illegal Function")},
    {ModbusErrorCode_Illegal_Data_Address, tr("Illegal Data Address")},
    {ModbusErrorCode_Illegal_Data_Value, tr("Illegal Data Value")},
    {ModbusErrorCode_Slave_Device_Failure, tr("Slave Device Failure")},
    {ModbusErrorCode_Acknowledge, tr("Acknowledge")},
    {ModbusErrorCode_Slave_Device_Busy, tr("Slave Device Busy")},
    {ModbusErrorCode_Negative_Acknowledgment, tr("Negative Acknowledgment")},
    {ModbusErrorCode_Memory_Parity_Error, tr("Memory Parity Error")},
    {ModbusErrorCode_Gateway_Path_Unavailable, tr("Gateway Path Unavailable")},
    {ModbusErrorCode_Gateway_Target_Device_Failed_To_Respond, tr("Gateway Target Device Failed To Respond")}
};
const QMap<ModbusErrorCode, QString> ModbusWidget::modbus_error_code_comment_map = {
    {ModbusErrorCode_Timeout, tr("The slave did not reply within the specified time.")},
    {ModbusErrorCode_Illegal_Function, tr("The function code received in the request is not an authorized action for the slave. The slave may be in the wrong state to process a specific request.")},
    {ModbusErrorCode_Illegal_Data_Address, tr("The data address received by the slave is not an authorized address for the slave.")},
    {ModbusErrorCode_Illegal_Data_Value, tr("The value in the request data field is not an authorized value for the slave.")},
    {ModbusErrorCode_Slave_Device_Failure, tr("The slave fails to perform a requested action because of an unrecoverable error.")},
    {ModbusErrorCode_Acknowledge, tr("The slave accepts the request but needs a long time to process it.")},
    {ModbusErrorCode_Slave_Device_Busy, tr("The slave is busy processing another command. The master must send the request once the slave is available.")},
    {ModbusErrorCode_Negative_Acknowledgment, tr("The slave cannot perform the programming request sent by the master.")},
    {ModbusErrorCode_Memory_Parity_Error, tr("The slave detects a parity error in the memory when attempting to read extended memory.")},
    {ModbusErrorCode_Gateway_Path_Unavailable, tr("The gateway is overloaded or not correctly configured.")},
    {ModbusErrorCode_Gateway_Target_Device_Failed_To_Respond, tr("The slave is not present on the network.")}
};

ModbusWidget::ModbusWidget(bool is_master, QIODevice *com,ModbusBase *modbus, int protocol, QMainWindow *parent_window, QWidget *parent)
    : ProtocolWidget(com, protocol, parent)
    , ui(new Ui::ModbusWidget), m_modbus(modbus), m_is_master(is_master), m_function05_dialog(nullptr)
    , m_function06_dialog(nullptr), m_function15_dialog(nullptr), m_function16_dialog(nullptr)
    , m_trans_id(0)
{
    ui->setupUi(this);

    m_parent_window = parent_window;

    m_recv_timeout_ms = 300;

    QVBoxLayout *v_layout = new QVBoxLayout(this);
    setLayout(v_layout);

    QMenuBar *menu_bar = new QMenuBar(this);
    v_layout->addWidget(menu_bar);
    QMenu *tool_menu = menu_bar->addMenu(tr("Tools"));
    QAction *add_reg_action = tool_menu->addAction(tr("Add Registers"));
    connect(add_reg_action, &QAction::triggered, this, &ModbusWidget::actionAddRegTriggered);
    QAction *modify_reg_action = tool_menu->addAction(tr("Modify Register Definitions"));
    connect(modify_reg_action, &QAction::triggered, this, &ModbusWidget::actionModifyRegDefTriggered);
    QAction *show_frame_action = tool_menu->addAction(tr("Display Communication"));
    connect(show_frame_action, &QAction::triggered, this, &ModbusWidget::actionDisplayTrafficTriggered);
    QAction *open_reg_def_file_action = tool_menu->addAction(tr("Open File"));
    connect(open_reg_def_file_action, &QAction::triggered, this, &ModbusWidget::openRegisterDefinitionFileTriggered);
    QAction *save_reg_def_action = tool_menu->addAction(tr("Save to file"));
    connect(save_reg_def_action, &QAction::triggered, this, &ModbusWidget::saveActivatingRegisterDifinitionsTriggered);
    QAction *save_all_reg_def_action = tool_menu->addAction(tr("Save All"));
    connect(save_all_reg_def_action, &QAction::triggered, this, &ModbusWidget::saveAllRegisterDefinitionsTriggered);

    QMenu *window_menu = menu_bar->addMenu(tr("Window"));
    QAction *cascade_window_action = window_menu->addAction(tr("Cascade"));
    connect(cascade_window_action, &QAction::triggered, this, &ModbusWidget::actionCascadeWindowTriggered);
    QAction *tile_window_action = window_menu->addAction(tr("Tile"));
    connect(tile_window_action, &QAction::triggered, this, &ModbusWidget::actionTileWindowTriggered);

    if(m_is_master)
    {
        m_plot_window = new PlotWindow(this);
        m_plot_window->hide();

        QAction *display_plot_window_action = tool_menu->addAction(tr("Show plot window"));
        connect(display_plot_window_action, &QAction::triggered, m_plot_window, &PlotWindow::show);

        m_test_center_window = new TestCenterWindow(m_com, this);
        m_test_center_window->hide();

        QAction *display_test_center_window_action = tool_menu->addAction(tr("Test Center"));
        connect(display_test_center_window_action, &QAction::triggered, this, &ModbusWidget::testCenterActionTriggered);
        connect(m_test_center_window, &TestCenterWindow::closed, this, &ModbusWidget::testCenterClosed);

        QAction *error_counter_action = tool_menu->addAction(tr("Error Counter"));
        connect(error_counter_action, &QAction::triggered, this, &ModbusWidget::actionErrorCounterTriggered);
        QMenu *setting_menu = menu_bar->addMenu(tr("Settings"));
        QAction *timeout_setting_action = setting_menu->addAction(tr("Timeout Setting"));
        connect(timeout_setting_action, &QAction::triggered, this, &ModbusWidget::actionSetRecvTimeoutTriggered);
        QMenu *functions_menu = menu_bar->addMenu(tr("Functions"));
        QAction *function_05_action = functions_menu->addAction(tr("05:Write Single Coil"));
        connect(function_05_action, &QAction::triggered, this, &ModbusWidget::actionFunction05Triggered);
        QAction *function_06_action = functions_menu->addAction(tr("06:Write Single Register"));
        connect(function_06_action, &QAction::triggered, this, &ModbusWidget::actionFunction06Triggered);
        QAction *function_15_action = functions_menu->addAction(tr("15:Write Coils"));
        connect(function_15_action, &QAction::triggered, this, &ModbusWidget::actionFunction15Triggered);
        QAction *function_16_action = functions_menu->addAction(tr("16:Write Registers"));
        connect(function_16_action, &QAction::triggered, this, &ModbusWidget::actionFunction16Triggered);

        m_error_counter_dialog = new ErrorCounterDialog(this);
        m_error_counter_dialog->hide();
    }
    else
    {
        m_error_counter_dialog = nullptr;
    }


    m_regs_area = new QMdiArea(this);
    m_regs_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_regs_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    v_layout->addWidget(m_regs_area);

    m_traffic_displayer = new DisplayCommunication(this);

    if(is_master)
    {
        m_scan_timer = new QTimer(this);
        connect(m_scan_timer, &QTimer::timeout, this, &ModbusWidget::scanTimerTimeoutSlot);
        m_scan_timer->start(1);
        m_scanning = true;
        m_send_timer = new QTimer(this);
        connect(m_send_timer, &QTimer::timeout, this, &ModbusWidget::sendTimerTimeoutSlot);
        m_send_timer->start(5);
        m_recv_timer = new QTimer(this);
        connect(m_recv_timer, &QTimer::timeout, this, &ModbusWidget::recvTimerTimeoutSlot);
        connect(m_com, &QIODevice::readyRead, this, &ModbusWidget::comMasterReadyReadSlot);
    }
    else
    {
        m_scanning = false;
        connect(m_com, &QIODevice::readyRead, this, &ModbusWidget::comSlaveReadyReadSlot);
    }
}

ModbusWidget::~ModbusWidget()
{
    delete ui;
    delete m_modbus;
}

void ModbusWidget::RegsViewWidgetClosed(ModbusRegReadDefinitions *reg_defines)
{
    m_reg_defines.removeOne(reg_defines);
    m_last_scan_timestamp_map.remove(reg_defines);
    RegsViewWidget *reg_view_widget = m_reg_def_widget_map[reg_defines];
    for(int i = 0; i < m_cycle_widget_list.size(); ++i)
    {
        if(m_cycle_widget_list[i] == reg_view_widget)
        {
            m_cycle_widget_list[i] = nullptr;
        }
    }
    m_reg_def_widget_map.remove(reg_defines);
    QList<register_value_t> graph_reg_list = m_plots_map[sender()];
    for(auto &reg : graph_reg_list)
    {
        m_plot_window->removeGraph(reg);
    }
    m_plots_map.remove(sender());
    delete reg_defines;
}

void ModbusWidget::writeFunctionTriggered(QByteArray pack)
{
    m_manual_list.append(pack);
}

void ModbusWidget::writeFrameTriggered(const ModbusFrameInfo &frame_info)
{
    QByteArray write_pack{};
    write_pack = m_modbus->masterFrame2Pack(frame_info);
    m_manual_list.append(write_pack);
}

void ModbusWidget::actionFunction05Triggered()
{
    if(!m_function05_dialog)
    {
        m_function05_dialog = new ModbusWriteSingleCoilDialog(m_modbus, this);
        connect(m_function05_dialog, &ModbusWriteSingleCoilDialog::writeFunctionTriggered, this, &ModbusWidget::writeFunctionTriggered);
        connect(this, &ModbusWidget::writeFunctionResponsed, m_function05_dialog, &ModbusWriteSingleCoilDialog::responseSlot);
        m_function05_dialog->show();
    }
    else
    {
        m_function05_dialog->show();
    }
}

void ModbusWidget::actionFunction06Triggered()
{
    if(!m_function06_dialog)
    {
        m_function06_dialog = new ModbusWriteSingleRegisterDialog(m_modbus, this);
        connect(m_function06_dialog, &ModbusWriteSingleRegisterDialog::writeFunctionTriggered, this, &ModbusWidget::writeFunctionTriggered);
        connect(this, &ModbusWidget::writeFunctionResponsed, m_function06_dialog, &ModbusWriteSingleRegisterDialog::responseSlot);
        m_function06_dialog->show();
    }
    else
    {
        m_function06_dialog->show();
    }
}

void ModbusWidget::actionFunction15Triggered()
{
    if(!m_function15_dialog)
    {
        m_function15_dialog = new ModbusWriteMultipleCoilsDialog(m_modbus, this);
        connect(m_function15_dialog, &ModbusWriteMultipleCoilsDialog::writeFunctionTriggered, this, &ModbusWidget::writeFunctionTriggered);
        connect(this, &ModbusWidget::writeFunctionResponsed, m_function15_dialog, &ModbusWriteMultipleCoilsDialog::responseSlot);
        m_function15_dialog->show();
    }
    else
    {
        m_function15_dialog->show();
    }
}

void ModbusWidget::actionFunction16Triggered()
{
    if(!m_function16_dialog)
    {
        m_function16_dialog = new ModbusWriteMultipleRegistersDialog(m_modbus, this);
        connect(m_function16_dialog, &ModbusWriteMultipleRegistersDialog::writeFunctionTriggered, this, &ModbusWidget::writeFunctionTriggered);
        connect(this, &ModbusWidget::writeFunctionResponsed, m_function16_dialog, &ModbusWriteMultipleRegistersDialog::responseSlot);
        m_function16_dialog->show();
    }
    else
    {
        m_function16_dialog->show();
    }
}

void ModbusWidget::actionModifyRegDefTriggered()
{
    QMdiSubWindow *act_sub_window = m_regs_area->activeSubWindow();
    if(!act_sub_window)
    {
        return;
    }
    RegsViewWidget *regs_view_widget = dynamic_cast<RegsViewWidget*>(act_sub_window->widget());
    if(regs_view_widget)
    {
        ModbusRegReadDefinitions *reg_def = getKeyByValue(m_reg_def_widget_map, regs_view_widget);
        if(reg_def)
        {
            AddRegDialog *modify_reg_dialog = new AddRegDialog(m_is_master, m_modbus, reg_def, this);
            connect(modify_reg_dialog, &AddRegDialog::readDefinitionsModified, this, std::bind(&ModbusWidget::modifyReadDefFinished, this, regs_view_widget, reg_def, std::placeholders::_1));
            modify_reg_dialog->show();
        }
    }
}

void ModbusWidget::actionAddRegTriggered()
{
    AddRegDialog *add_reg_dialog = new AddRegDialog(m_is_master, m_modbus, this);
    connect(add_reg_dialog, &AddRegDialog::readDefinitionsCreated, this, &ModbusWidget::regDefinitionsCreated);
    add_reg_dialog->show();
}

void ModbusWidget::actionSetRecvTimeoutTriggered()
{
    bool input_ok {false};
    int recv_timeout = QInputDialog::getInt(this,
                                            tr("Set Modbus Receive Timeout(ms)"),
                                            tr("Modbus Receive Timeout(ms) : "),
                                            m_recv_timeout_ms,
                                            100,
                                            800,
                                            1,
                                            &input_ok);
    if(input_ok)
    {
        m_recv_timeout_ms = recv_timeout;
    }
}

void ModbusWidget::actionDisplayTrafficTriggered()
{
    m_traffic_displayer->setWindowTitle(QString("Communication Traffic - %1").arg(windowTitle()));
    m_traffic_displayer->show();
}

void ModbusWidget::actionErrorCounterTriggered()
{
    if(m_error_counter_dialog)
    {
        m_error_counter_dialog->show();
    }
}

void ModbusWidget::actionCascadeWindowTriggered()
{
    m_regs_area->cascadeSubWindows();
}

void ModbusWidget::actionTileWindowTriggered()
{
    m_regs_area->tileSubWindows();
}

void ModbusWidget::regDefinitionsCreated(ModbusRegReadDefinitions *reg_defines)
{
    if(validRegsDefinition(reg_defines))
    {
        m_reg_defines.append(reg_defines);
        RegsViewWidget *regs_view_widget = new RegsViewWidget(reg_defines,this);
        connect(regs_view_widget, &RegsViewWidget::writeFunctionTriggered, this, &ModbusWidget::writeFrameTriggered);
        connect(regs_view_widget, &RegsViewWidget::closed, this, &ModbusWidget::RegsViewWidgetClosed);
        connect(regs_view_widget, &RegsViewWidget::append_plot_graph, this, &ModbusWidget::appendPlotGraphSlot);
        regs_view_widget->setWindowTitle(QString("ID:%1 - Registers : %2").arg(reg_defines->id).arg(reg_defines->reg_addr));
        m_last_scan_timestamp_map[reg_defines] = 0;
        m_reg_def_widget_map[reg_defines] = regs_view_widget;
        m_regs_area->addSubWindow(regs_view_widget);
        regs_view_widget->show();
    }
    else
    {
        FloatBox::message("Illegal Registers Definition, Check collision", 3000, m_parent_window->geometry());
        delete reg_defines;
    }

}

void ModbusWidget::scanTimerTimeoutSlot()
{
    if(!m_cycle_list.isEmpty() || m_cycle_list.size() > CYCLE_QUEUE_MAX_SIZE || !m_scanning)
    {
        return;
    }
    qint64 now_timestamp = QDateTime::currentMSecsSinceEpoch();
    for(auto &x : m_reg_defines)
    {
        if(now_timestamp - m_last_scan_timestamp_map[x] >= x->scan_rate)
        {
            m_cycle_list.append(x->packet);
            m_cycle_widget_list.append(m_reg_def_widget_map[x]);
            m_last_scan_timestamp_map[x] = now_timestamp;
        }
    }
}

void ModbusWidget::sendTimerTimeoutSlot()
{
    bool has_pack{false};
    if(!m_manual_list.isEmpty())
    {
        has_pack = true;
        m_master_last_send_pack = m_manual_list.first();
    }
    else if(!m_cycle_list.isEmpty())
    {
        has_pack = true;
        m_master_last_send_pack = m_cycle_list.first();
        RegsViewWidget *regs_view_widget = m_cycle_widget_list.first();
        regs_view_widget->increaseSendCount();
    }
    if(has_pack)
    {
#if PRINT_TRAFFIC
        qDebug()<<"Master Send: "<<m_master_last_send_pack.toHex(' ').toUpper();
#endif
        if(m_protocol == MODBUS_TCP || m_protocol == MODBUS_UDP)
        {
            setModbusPacketTransID(m_master_last_send_pack, m_trans_id);
            ++m_trans_id;
        }
        m_com->write(m_master_last_send_pack);
        QString traffic_str = QString("Tx: %1").arg(m_master_last_send_pack.toHex(' ').toUpper());
        if(m_traffic_displayer->isVisible())
        {
            m_traffic_displayer->appendPacket(traffic_str, false);
        }
        m_send_timer->stop();
        m_recv_timer->start(m_recv_timeout_ms);
    }
}

void ModbusWidget::recvTimerTimeoutSlot()
{
    m_recv_timer->stop();
    RegsViewWidget *regs_view_widget {nullptr};
    if(!m_manual_list.isEmpty())
    {
        m_manual_list.removeFirst();
    }
    else if(!m_cycle_list.isEmpty())
    {
        m_cycle_list.removeFirst();
        regs_view_widget = m_cycle_widget_list.takeFirst();
    }
    m_master_last_send_frame = m_modbus->slavePack2Frame(m_master_last_send_pack);
    if(m_master_last_send_frame.function == ModbusWriteSingleCoil ||
        m_master_last_send_frame.function == ModbusWriteMultipleCoils ||
        m_master_last_send_frame.function == ModbusWriteSingleRegister ||
        m_master_last_send_frame.function == ModbusWriteMultipleRegisters)
    {
        emit writeFunctionResponsed(ModbusErrorCode_Timeout);
    }
    if(!m_recv_buffer.isEmpty())
    {
        m_recv_buffer.clear();
    }
    if(m_error_counter_dialog)
    {
        m_error_counter_dialog->increaseErrorCount(ModbusErrorCode_Timeout);
    }
    if(regs_view_widget)
    {
        regs_view_widget->increaseErrorCount();
        regs_view_widget->setErrorInfo(tr("Timeout Error"));
    }
    if(m_scanning)
    {
        m_send_timer->start();
    }
}

void ModbusWidget::comMasterReadyReadSlot()
{
    m_recv_buffer.append(m_com->readAll());
    bool is_intact {false};
    ModbusFrameInfo frame_info{};
    is_intact = m_modbus->validPack(m_recv_buffer);
    if(is_intact)
    {
        frame_info = m_modbus->masterPack2Frame(m_recv_buffer);
        m_master_last_send_frame = m_modbus->slavePack2Frame(m_master_last_send_pack);
#if PRINT_TRAFFIC
        qDebug()<<"Master Recv: "<<m_recv_buffer.toHex(' ').toUpper();
#endif
        if(frame_info.id == m_master_last_send_frame.id)
        {
            if(((m_protocol == MODBUS_TCP || m_protocol == MODBUS_UDP) && frame_info.trans_id == m_master_last_send_frame.trans_id)
                || (m_protocol != MODBUS_TCP && m_protocol != MODBUS_UDP))
            {
                QString traffic_str = QString("Rx: %1").arg(m_recv_buffer.toHex(' ').toUpper());
                if(m_traffic_displayer->isVisible())
                {
                    m_traffic_displayer->appendPacket(traffic_str, frame_info.function > ModbusFunctionError);
                }
                m_recv_timer->stop();
                processModbusFrame(frame_info);
                if(m_scanning)
                {
                    m_send_timer->start(5);
                }
            }
        }
        m_recv_buffer.clear();
    }
}

void ModbusWidget::comSlaveReadyReadSlot()
{
    if(m_protocol == MODBUS_RTU)
    {
        static qint64 last_recved_timestamp = QDateTime::currentMSecsSinceEpoch();
        qint64 now_timestamp = QDateTime::currentMSecsSinceEpoch();
        if(now_timestamp - last_recved_timestamp > 5)
        {
            m_recv_buffer.clear();
        }
        last_recved_timestamp = now_timestamp;
    }
    m_recv_buffer.append(m_com->readAll());
    bool is_intact {false};
    ModbusFrameInfo frame_info{};
    is_intact = m_modbus->validPack(m_recv_buffer);
    if(is_intact)
    {
        frame_info = m_modbus->slavePack2Frame(m_recv_buffer);
#if PRINT_TRAFFIC
        qDebug()<<"Slave Recv: "<<m_recv_buffer.toHex(' ').toUpper();
#endif
        bool has_id{false};
        for(auto &x : m_reg_defines)
        {
            if(x->id == frame_info.id)
            {
                has_id = true;
            }
        }
        if(has_id)
        {
            QString traffic_str = QString("Rx: %1").arg(m_recv_buffer.toHex(' ').toUpper());
            if(m_traffic_displayer->isVisible())
            {
                m_traffic_displayer->appendPacket(traffic_str, false);
            }
            processModbusFrame(frame_info);
        }
        m_recv_buffer.clear();
    }
}

void ModbusWidget::modifyReadDefFinished(RegsViewWidget *regs_view_widget, ModbusRegReadDefinitions *old_def, ModbusRegReadDefinitions *new_def)
{
    if(validRegsDefinition(new_def))
    {
        m_reg_defines.removeOne(old_def);
        m_reg_defines.append(new_def);
        quint64 last_scan_timestamp = m_last_scan_timestamp_map[old_def];
        m_last_scan_timestamp_map.remove(old_def);
        m_last_scan_timestamp_map[new_def] = last_scan_timestamp;
        m_reg_def_widget_map.remove(old_def);
        m_reg_def_widget_map[new_def] = regs_view_widget;
        delete old_def;
        regs_view_widget->setRegDef(new_def);
    }
}

void ModbusWidget::appendPlotGraphSlot(register_value_t reg_val)
{
    if(m_plot_window->isFull())
    {
        static bool first_warn = true;
        if(first_warn)
        {
            QMessageBox::information(this, tr("tips"), tr("Only ten graphs can be displayed at most."));
            first_warn = false;
        }
        return;
    }
    m_plot_window->addGraph(reg_val);
    m_plot_window->show();
    if(!m_plots_map[sender()].contains(reg_val))
    {
        m_plots_map[sender()].append(reg_val);
    }
}

void ModbusWidget::testCenterActionTriggered()
{
    disconnect(m_com, &QIODevice::readyRead, this, &ModbusWidget::comMasterReadyReadSlot);
    m_test_center_window->show();
    m_scanning = false;
}

void ModbusWidget::testCenterClosed()
{
    connect(m_com, &QIODevice::readyRead, this, &ModbusWidget::comMasterReadyReadSlot);
    m_scanning = true;
    m_send_timer->start(5);
}

void ModbusWidget::openRegisterDefinitionFileTriggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open"), QString(), "JSON files(*.json)");
    if(file_name.isEmpty())
    {
        return;
    }
    QFile reg_def_file(file_name);
    if(reg_def_file.open(QIODevice::ReadOnly))
    {
        ModbusRegReadDefinitions *reg_def = new ModbusRegReadDefinitions;
        QList<QString> reg_alias;
        QList<quint16> reg_values;
        QList<qint32> reg_formats;
        if(readRegisterDefinitionFile(reg_def, reg_def_file, reg_alias, reg_values, reg_formats))
        {
            regDefinitionsCreated(reg_def);
            RegsViewWidget *reg_view_widget = m_reg_def_widget_map[reg_def];
            for(int i = 0; i < reg_alias.size(); ++i)
            {
                reg_view_widget->setRegisterAlias(i, reg_alias[i]);
                reg_view_widget->setRegisterFormat(i, reg_formats[i]);
                if(!reg_def->is_master)
                {
                    reg_view_widget->setRegisterValues(reg_values.data(), reg_def->reg_addr, reg_def->quantity);
                }
            }
        }
        else
        {
            delete reg_def;
        }
    }
}

void ModbusWidget::saveAllRegisterDefinitionsTriggered()
{
    for(auto &x : m_reg_defines)
    {
        QString file_name = QFileDialog::getSaveFileName(this, tr("Save"), QString(), "JSON file(*.json)");
        if(file_name.isEmpty())
        {
            FloatBox::message(tr("Save Cancelled"), 3, m_parent_window->geometry());
            continue;
        }
        QFile reg_def_file(file_name);
        if(reg_def_file.open(QIODevice::WriteOnly))
        {
            saveRegisterDefinition(x, reg_def_file);
        }
    }
}

void ModbusWidget::saveActivatingRegisterDifinitionsTriggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save"), QString(), "Json file(*.txt)");
    QMdiSubWindow *activate_window = m_regs_area->activeSubWindow();
    if(activate_window == nullptr || file_name.isEmpty())
    {
        FloatBox::message(tr("Save Cancelled"), 3, m_parent_window->geometry());
        return;
    }
    RegsViewWidget *reg_view_widget = dynamic_cast<RegsViewWidget*>(activate_window->widget());
    ModbusRegReadDefinitions *reg_def = m_reg_def_widget_map.key(reg_view_widget);
    QFile reg_def_file(file_name);
    if(reg_def_file.open(QIODevice::WriteOnly))
    {
        saveRegisterDefinition(reg_def, reg_def_file);
    }
}

void ModbusWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    for(auto &x : m_reg_defines)
    {
        delete x;
    }
    deleteLater();
}

bool ModbusWidget::validRegsDefinition(ModbusRegReadDefinitions *reg_def)
{
    for(auto &x : m_reg_defines)
    {
        if(reg_def->id == x->id)
        {
            if(reg_def->reg_addr >= x->reg_addr)
            {
                if(reg_def->reg_addr < x->reg_addr + x->quantity)
                {
                    return false;
                }
            }
            else
            {
                if(x->reg_addr < reg_def->reg_addr + reg_def->quantity)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

ModbusRegReadDefinitions *ModbusWidget::getSlaveReadDefinitions(int id, int function, int reg_addr, int quantity, ModbusErrorCode &error_code)
{
    bool found_id {false};
    for(auto &x : m_reg_defines)
    {
        if(x->id == id)
        {
            found_id = true;
            if(reg_addr >= x->reg_addr)
            {
                if(reg_addr + quantity <= x->reg_addr + x->quantity)
                {
                    if(x->function == function ||
                        ((function == ModbusWriteSingleCoil || function == ModbusWriteMultipleCoils) && x->function == ModbusCoilStatus) ||
                        ((function == ModbusWriteSingleRegister || function ==ModbusWriteMultipleRegisters) && x->function == ModbusHoldingRegisters))
                    {
                        error_code = ModbusErrorCode_OK;
                        return x;
                    }
                    else
                    {
                        error_code = ModbusErrorCode_Illegal_Function;
                        return nullptr;
                    }
                }
            }
        }
    }
    if(found_id)
    {
        error_code = ModbusErrorCode_Illegal_Data_Address;
    }
    else
    {
        error_code = ModbusErrorCode_OK;
    }
    return nullptr;
}

void ModbusWidget::processModbusFrame(const ModbusFrameInfo &frame_info)
{
    if(m_is_master)
    {
        RegsViewWidget *regs_view_widget{nullptr};
        if(!m_manual_list.isEmpty())
        {
            m_manual_list.removeFirst();
        }
        else if(!m_cycle_list.isEmpty())
        {
            m_cycle_list.removeFirst();
            regs_view_widget = m_cycle_widget_list.takeFirst();
        }
        if(frame_info.function > ModbusFunctionError)
        {
            ModbusErrorCode error_code = (ModbusErrorCode)(frame_info.reg_values[0]);
            int func_code = frame_info.function - ModbusFunctionError;
            if(func_code == ModbusWriteSingleCoil ||
                func_code == ModbusWriteMultipleCoils ||
                func_code == ModbusWriteSingleRegister ||
                func_code == ModbusWriteMultipleRegisters)
            {
                emit writeFunctionResponsed(error_code);
            }
            if(m_error_counter_dialog)
            {
                m_error_counter_dialog->increaseErrorCount(error_code);
            }
            if(regs_view_widget)
            {
                regs_view_widget->increaseErrorCount();
                regs_view_widget->setErrorInfo(modbus_error_code_map[error_code]);
            }
        }
        else if(frame_info.function == ModbusReadCoils ||
                 frame_info.function == ModbusReadDescreteInputs)
        {
            if(regs_view_widget)
            {
                quint8 *coils = (quint8*)frame_info.reg_values;
                for(int i = 0; i < m_master_last_send_frame.quantity; ++i)
                {
                    int byte_index = i / 8;
                    int bit_index = i % 8;
                    regs_view_widget->setCoilValue(m_master_last_send_frame.reg_addr + i, getBit(coils[byte_index], bit_index));
                }
                regs_view_widget->setErrorInfo("");
            }
        }
        else if(frame_info.function == ModbusReadHoldingRegisters ||
                   frame_info.function == ModbusReadInputRegisters)
        {
            if(regs_view_widget)
            {
                regs_view_widget->setRegisterValues(frame_info.reg_values, m_master_last_send_frame.reg_addr, frame_info.quantity);
                m_plot_window->flushGraph();
                regs_view_widget->setErrorInfo("");
            }
        }
        else if(m_master_last_send_frame.function == ModbusWriteSingleCoil ||
                   m_master_last_send_frame.function == ModbusWriteMultipleCoils ||
                   m_master_last_send_frame.function == ModbusWriteSingleRegister ||
                   m_master_last_send_frame.function == ModbusWriteMultipleRegisters)
        {
            emit writeFunctionResponsed(ModbusErrorCode_OK);
            if(regs_view_widget)
            {
                regs_view_widget->setErrorInfo("");
            }
        }
        else
        {
            qDebug()<<"Unkown Modbus Function Code : "<<frame_info.function <<frame_info.id <<frame_info.reg_addr<<frame_info.quantity;
        }
    }
    else
    {
        ModbusErrorCode error_code{ModbusErrorCode_OK};
        ModbusRegReadDefinitions *reg_def = getSlaveReadDefinitions(frame_info.id, frame_info.function, frame_info.reg_addr,frame_info.quantity,error_code);
        ModbusFrameInfo reply_frame{};
        reply_frame.id = frame_info.id;
        reply_frame.trans_id = frame_info.trans_id;
        if(reg_def)
        {
            reply_frame.function = frame_info.function;
            reply_frame.reg_addr = frame_info.reg_addr;
            reply_frame.quantity = frame_info.quantity;
            RegsViewWidget *regs_view_widget = m_reg_def_widget_map[reg_def];
            if(frame_info.function == ModbusReadHoldingRegisters || frame_info.function == ModbusReadInputRegisters)
            {
                regs_view_widget->getRegisterValues(reply_frame.reg_values,reply_frame.reg_addr,reply_frame.quantity);
            }
            else if(frame_info.function == ModbusReadCoils || frame_info.function == ModbusReadDescreteInputs)
            {
                quint8 *coils = (quint8*)reply_frame.reg_values;
                for(int i = 0;i < reply_frame.quantity;++i)
                {
                    quint16 value {0};
                    regs_view_widget->getCoilValue(reply_frame.reg_addr + i,&value);
                    int byte_index = i / 8;
                    int bit_index = i % 8;
                    setBit(coils[byte_index], bit_index, value);
                }
            }
            else if(frame_info.function == ModbusWriteSingleCoil)
            {

                reply_frame.reg_values[0] = frame_info.reg_values[0];
                regs_view_widget->setCoilValue(frame_info.reg_addr, frame_info.reg_values[0] >> 8 & 0xFF ? 1 : 0);
            }
            else if(frame_info.function == ModbusWriteMultipleCoils)
            {
                quint8 *coils = (quint8*)frame_info.reg_values;
                for(int i = 0;i < frame_info.quantity;++i)
                {
                    int byte_index = i / 8;
                    int bit_index = i % 8;
                    regs_view_widget->setCoilValue(frame_info.reg_addr + i, getBit(coils[byte_index], bit_index));
                }
            }
            else if(frame_info.function == ModbusWriteSingleRegister)
            {
                reply_frame.reg_values[0] = frame_info.reg_values[0];
                regs_view_widget->setRegisterValues(frame_info.reg_values, frame_info.reg_addr, 1);
            }
            else if(frame_info.function == ModbusWriteMultipleRegisters)
            {
                regs_view_widget->setRegisterValues(frame_info.reg_values, frame_info.reg_addr, frame_info.quantity);
            }
        }
        else
        {
            reply_frame.function = frame_info.function + ModbusFunctionError;
            reply_frame.reg_values[0] = error_code;
        }
        QByteArray reply_pack;
        reply_pack = m_modbus->slaveFrame2Pack(reply_frame);
#if PRINT_TRAFFIC
        qDebug()<<"Slave Send: "<<reply_pack.toHex(' ').toUpper();
#endif
        m_com->write(reply_pack);
        QString traffic_str = QString("Tx: %1").arg(reply_pack.toHex(' ').toUpper());
        if(m_traffic_displayer->isVisible())
        {
            m_traffic_displayer->appendPacket(traffic_str, reply_frame.function > ModbusFunctionError);
        }
    }
}

void ModbusWidget::saveRegisterDefinition(ModbusRegReadDefinitions *reg_def, QFile &file)
{
    QJsonDocument json_doc;
    QJsonObject reg_def_obj;
    RegsViewWidget *reg_view_widget = m_reg_def_widget_map[reg_def];
    reg_def_obj.insert(REG_DEF_IS_MASTER_STR, reg_def->is_master);
    reg_def_obj.insert(REG_DEF_ID_STR, reg_def->id);
    reg_def_obj.insert(REG_DEF_FUNCTION_STR, reg_def->function);
    reg_def_obj.insert(REG_DEF_QUANTITY_STR, reg_def->quantity);
    reg_def_obj.insert(REG_DEF_ADDR_STR, reg_def->reg_addr);
    QJsonArray reg_arr;
    if(reg_def->is_master)
    {
        reg_def_obj.insert(REG_DEF_SR_STR, reg_def->scan_rate);
        reg_def_obj.insert(REG_DEF_PACK_STR, QString(reg_def->packet.toHex()));
        for(int i = 0; i < reg_def->quantity; ++i)
        {
            QJsonObject reg_obj;
            reg_obj.insert(REG_DEF_ADDR_OFFSET_STR, i);
            reg_obj.insert(REG_DEF_ADDR_ALIAS_STR, reg_view_widget->getRegisterAlias(i));
            reg_obj.insert(REG_DEF_ADDR_FORMAT_STR, reg_view_widget->getRegisterFormat(i));
            reg_arr.append(reg_obj);
        }
    }
    else
    {
        quint16 *reg_values = new quint16[reg_def->quantity];
        reg_view_widget->getRegisterValues(reg_values, reg_def->reg_addr, reg_def->quantity);
        for(int i = 0; i < reg_def->quantity; ++i)
        {
            QJsonObject reg_obj;
            reg_obj.insert(REG_DEF_ADDR_OFFSET_STR, i);
            reg_obj.insert(REG_DEF_ADDR_ALIAS_STR, reg_view_widget->getRegisterAlias(i));
            reg_obj.insert(REG_DEF_ADDR_FORMAT_STR, reg_view_widget->getRegisterFormat(i));
            reg_obj.insert(REG_DEF_ADDR_VALUE_STR, reg_values[i]);
            reg_arr.append(reg_obj);
        }
    }
    reg_def_obj.insert(REG_DEF_REG_INFO_STR, reg_arr);
    json_doc.setObject(reg_def_obj);
    file.write(json_doc.toJson());
}

bool ModbusWidget::readRegisterDefinitionFile(ModbusRegReadDefinitions *reg_def, QFile &file, QList<QString> &reg_alias, QList<quint16> &reg_values, QList<qint32> &reg_formats)
{
    QJsonParseError parse_err{};
    QJsonDocument json_doc = QJsonDocument::fromJson(file.readAll(), &parse_err);
    if(parse_err.error != QJsonParseError::NoError)
    {
        FloatBox::message(parse_err.errorString(), 3000, m_parent_window->geometry());
        return false;
    }
    QJsonObject reg_def_obj = json_doc.object();
    reg_def->is_master = reg_def_obj.value(REG_DEF_IS_MASTER_STR).toBool();
    reg_def->id = reg_def_obj.value(REG_DEF_ID_STR).toInt();
    reg_def->function = reg_def_obj.value(REG_DEF_FUNCTION_STR).toInt();
    reg_def->quantity = reg_def_obj.value(REG_DEF_QUANTITY_STR).toInt();
    reg_def->reg_addr = reg_def_obj.value(REG_DEF_ADDR_STR).toInt();
    QJsonArray reg_arr = reg_def_obj.value(REG_DEF_REG_INFO_STR).toArray();
    reg_alias.resize(reg_def->quantity);
    reg_values.resize(reg_def->quantity);
    reg_formats.resize(reg_def->quantity);
    if(reg_def->is_master)
    {
        reg_def->scan_rate = reg_def_obj.value(REG_DEF_SR_STR).toInt();
        reg_def->packet = QByteArray::fromHex(reg_def_obj.value(REG_DEF_PACK_STR).toString().toUtf8());
        for(int i = 0; i < reg_def->quantity; ++i)
        {
            QJsonObject reg_obj = reg_arr[i].toObject();
            qint32 reg_offset = reg_obj.value(REG_DEF_ADDR_OFFSET_STR).toInt();
            reg_alias[reg_offset] = reg_obj.value(REG_DEF_ADDR_ALIAS_STR).toString();
            reg_formats[reg_offset] = reg_obj.value(REG_DEF_ADDR_FORMAT_STR).toInt();
        }
    }
    else
    {
        for(int i = 0; i < reg_def->quantity; ++i)
        {
            QJsonObject reg_obj = reg_arr[i].toObject();
            qint32 reg_offset = reg_obj.value(REG_DEF_ADDR_OFFSET_STR).toInt();
            reg_alias[reg_offset] = reg_obj.value(REG_DEF_ADDR_ALIAS_STR).toString();
            reg_formats[reg_offset] = reg_obj.value(REG_DEF_ADDR_FORMAT_STR).toInt();
            reg_values[reg_offset] = reg_obj.value(REG_DEF_ADDR_VALUE_STR).toInt();
        }
    }
    return true;
}

