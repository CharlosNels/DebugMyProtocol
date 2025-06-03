#ifndef REGSVIEWWIDGET_H
#define REGSVIEWWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMap>
#include "EnumHeader.h"
#include "register_value_t.h"

struct ModbusRegReadDefinitions;

namespace Ui {
class RegsViewWidget;
}

class QMenu;
class QAction;
struct ModbusFrameInfo;

class RegsViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegsViewWidget(ModbusRegReadDefinitions *reg_defines, QWidget *parent = nullptr);
    ~RegsViewWidget();
    void setRegDef(ModbusRegReadDefinitions *reg_defines);
    void increaseErrorCount();
    void increaseSendCount();
    void setErrorInfo(const QString &error_info);
    bool setRegisterValues(const quint16 *reg_values, quint16 reg_addr, quint16 quantity);
    bool getRegisterValues(quint16 *reg_values, quint16 reg_addr, quint16 quantity) const;
    bool setCoilValue(int coil_addr, quint16 value);
    bool getCoilValue(int coil_addr, quint16 *value) const;
    QString getRegisterAlias(qint32 reg_offset);
    void setRegisterAlias(qint32 reg_offset, const QString &alias);
    qint32 getRegisterFormat(qint32 reg_offset);
    void setRegisterFormat(qint32 reg_offset, qint32 format);

signals:
    void writeFunctionTriggered(const ModbusFrameInfo &frame_info);
    void closed(ModbusRegReadDefinitions *reg_defines);
    void append_plot_graph(register_value_t reg_val);

private slots:
    void formatActionTriggered();
    void copyActionTriggered();
    void selectAllActionTriggered();
    void appendToPlotsActionTriggered();

    void on_regs_table_view_customContextMenuRequested(const QPoint &pos);

    void on_regs_table_view_doubleClicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event) override;

private:



    Ui::RegsViewWidget *ui;
    QStandardItemModel *m_table_model;
    ModbusRegReadDefinitions *m_reg_defines;
    quint32 m_send_count;
    quint32 m_error_count;
    quint16 *m_register_values;
    QList<CellFormat> m_cell_formats;
    QMap<QAction *, CellFormat> m_format_map;
    QMenu *m_popup_menu;

    QAction *m_format_signed_action;
    QAction *m_format_unsigned_action;
    QAction *m_format_hex_action;
    QAction *m_format_ascii_hex_action;
    QAction *m_format_binary_action;
    QAction *m_format_32_bit_signed_big_endian_action;
    QAction *m_format_32_bit_signed_little_endian_action;
    QAction *m_format_32_bit_signed_big_endian_byte_swap_action;
    QAction *m_format_32_bit_signed_little_endian_byte_swap_action;
    QAction *m_format_32_bit_unsigned_big_endian_action;
    QAction *m_format_32_bit_unsigned_little_endian_action;
    QAction *m_format_32_bit_unsigned_big_endian_byte_swap_action;
    QAction *m_format_32_bit_unsigned_little_endian_byte_swap_action;
    QAction *m_format_64_bit_signed_big_endian_action;
    QAction *m_format_64_bit_signed_little_endian_action;
    QAction *m_format_64_bit_signed_big_endian_byte_swap_action;
    QAction *m_format_64_bit_signed_little_endian_byte_swap_action;
    QAction *m_format_64_bit_unsigned_big_endian_action;
    QAction *m_format_64_bit_unsigned_little_endian_action;
    QAction *m_format_64_bit_unsigned_big_endian_byte_swap_action;
    QAction *m_format_64_bit_unsigned_little_endian_byte_swap_action;
    QAction *m_format_32_bit_float_big_endian_action;
    QAction *m_format_32_bit_float_little_endian_action;
    QAction *m_format_32_bit_float_big_endian_byte_swap_action;
    QAction *m_format_32_bit_float_little_endian_byte_swap_action;
    QAction *m_format_64_bit_float_big_endian_action;
    QAction *m_format_64_bit_float_little_endian_action;
    QAction *m_format_64_bit_float_big_endian_byte_swap_action;
    QAction *m_format_64_bit_float_little_endian_byte_swap_action;

    QAction *m_copy_action;
    QAction *m_select_all_action;

    QAction *m_append_to_plot_graph_action;
    
private:
    void updateRegisterValues();

};

#endif // REGSVIEWWIDGET_H
