#ifndef MODBUSWIDGET_H
#define MODBUSWIDGET_H

#include "protocolwidget.h"
#include <QMdiArea>
#include <QList>
#include "ModbusFrameInfo.h"
#include "modbuswritesinglecoildialog.h"
#include "modbuswritesingleregisterdialog.h"
#include "modbuswritemultiplecoilsdialog.h"
#include "modbuswritemultipleregistersdialog.h"
#include "ModbusBase.h"

struct ModbusRegReadDefinitions;
class QTimer;
class RegsViewWidget;
class DisplayCommunication;
class ErrorCounterDialog;

namespace Ui {
class ModbusWidget;
}

class ModbusWidget : public ProtocolWidget
{
    Q_OBJECT

public:
    explicit ModbusWidget(bool is_master, QIODevice *com,ModbusBase *modbus, int protocol, QWidget *parent = nullptr);
    ~ModbusWidget();

signals:
    void writeFunctionResponsed(int error_code);

private slots:
    void RegsViewWidgetClosed(ModbusRegReadDefinitions *reg_defines);
    void writeFunctionTriggered(QByteArray pack);
    void writeFrameTriggered(const ModbusFrameInfo &frame_info);
    void actionFunction05Triggered();
    void actionFunction06Triggered();
    void actionFunction15Triggered();
    void actionFunction16Triggered();
    void actionModifyRegDefTriggered();
    void actionAddRegTriggered();
    void actionSetRecvTimeoutTriggered();
    void actionDisplayTrafficTriggered();
    void actionErrorCounterTriggered();
    void actionCascadeWindowTriggered();
    void actionTileWindowTriggered();
    void regDefinitionsCreated(ModbusRegReadDefinitions *reg_defines);
    void scanTimerTimeoutSlot();
    void sendTimerTimeoutSlot();
    void recvTimerTimeoutSlot();
    void comMasterReadyReadSlot();
    void comSlaveReadyReadSlot();
    void modifyReadDefFinished(RegsViewWidget *regs_view_widget, ModbusRegReadDefinitions *old_def, ModbusRegReadDefinitions *new_def);

private:
    bool validRegsDefinition(ModbusRegReadDefinitions *reg_def);
    ModbusRegReadDefinitions *getSlaveReadDefinitions(int id, int function, int reg_addr, int quantity, ModbusErrorCode &error_code);
    void processModbusFrame(const ModbusFrameInfo &frame_info);

private:
    Ui::ModbusWidget *ui;
    QMdiArea *m_regs_area;
    ModbusBase *m_modbus;
    bool m_is_master;
    QTimer *m_scan_timer;
    QTimer *m_send_timer;
    QTimer *m_recv_timer;
    QByteArray m_recv_buffer;
    QByteArray m_master_last_send_pack;
    ModbusFrameInfo m_master_last_send_frame;
    QList<ModbusRegReadDefinitions*> m_reg_defines;
    QMap<ModbusRegReadDefinitions*,quint64> m_last_scan_timestamp_map;
    QMap<ModbusRegReadDefinitions*,RegsViewWidget*> m_reg_def_widget_map;
    QList<QByteArray> m_cycle_list;
    QList<RegsViewWidget*> m_cycle_widget_list;
    QList<QByteArray> m_manual_list;
    quint32 m_recv_timeout_ms;
    DisplayCommunication *m_traffic_displayer;
    ModbusWriteSingleCoilDialog *m_function05_dialog;
    ModbusWriteSingleRegisterDialog *m_function06_dialog;
    ModbusWriteMultipleCoilsDialog *m_function15_dialog;
    ModbusWriteMultipleRegistersDialog *m_function16_dialog;
    quint16 m_trans_id;
    ErrorCounterDialog *m_error_counter_dialog;

public:
    static const QMap<ModbusErrorCode, QString> modbus_error_code_map;
    static const QMap<ModbusErrorCode, QString> modbus_error_code_comment_map;

};

#endif // MODBUSWIDGET_H
