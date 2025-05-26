#ifndef REGSVIEWWIDGET_H
#define REGSVIEWWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMap>

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
    enum CellFormat{
        Format_None = 0,
        Format_Coil,
        Format_Signed,
        Format_Unsigned,
        Format_Hex,
        Format_Ascii_Hex,
        Format_Binary,
        Format_32_Bit_Signed_Big_Endian = 32,
        Format_32_Bit_Signed_Little_Endian,
        Format_32_Bit_Signed_Big_Endian_Byte_Swap,
        Format_32_Bit_Signed_Little_Endian_Byte_Swap,
        Format_32_Bit_Unsigned_Big_Endian,
        Format_32_Bit_Unsigned_Little_Endian,
        Format_32_Bit_Unsigned_Big_Endian_Byte_Swap,
        Format_32_Bit_Unsigned_Little_Endian_Byte_Swap,
        Format_32_Bit_Float_Big_Endian,
        Format_32_Bit_Float_Little_Endian,
        Format_32_Bit_Float_Big_Endian_Byte_Swap,
        Format_32_Bit_Float_Little_Endian_Byte_Swap,
        Format_64_Bit_Signed_Big_Endian = 64,
        Format_64_Bit_Signed_Little_Endian,
        Format_64_Bit_Signed_Big_Endian_Byte_Swap,
        Format_64_Bit_Signed_Little_Endian_Byte_Swap,
        Format_64_Bit_Unsigned_Big_Endian,
        Format_64_Bit_Unsigned_Little_Endian,
        Format_64_Bit_Unsigned_Big_Endian_Byte_Swap,
        Format_64_Bit_Unsigned_Little_Endian_Byte_Swap,
        Format_64_Bit_Float_Big_Endian,
        Format_64_Bit_Float_Little_Endian,
        Format_64_Bit_Float_Big_Endian_Byte_Swap,
        Format_64_Bit_Float_Little_Endian_Byte_Swap,
    };

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

signals:
    void writeFunctionTriggered(const ModbusFrameInfo &frame_info);
    void closed(ModbusRegReadDefinitions *reg_defines);

private slots:
    void formatActionTriggered();
    void copyActionTriggered();
    void selectAllActionTriggered();

    void on_regs_table_view_customContextMenuRequested(const QPoint &pos);

    void on_regs_table_view_doubleClicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event)override;

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
    
private:
    void updateRegisterValues();

};

#endif // REGSVIEWWIDGET_H
