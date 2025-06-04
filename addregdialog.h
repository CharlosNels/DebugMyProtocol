#ifndef ADDREGDIALOG_H
#define ADDREGDIALOG_H

#include <QDialog>
#include <QMap>


namespace Ui {
class AddRegDialog;
}

class ModbusBase;

struct ModbusRegReadDefinitions;

class AddRegDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddRegDialog(bool is_master, ModbusBase *modbus, QWidget *parent = nullptr);
    explicit AddRegDialog(bool is_master, ModbusBase *modbus, ModbusRegReadDefinitions *reg_def, QWidget *parent = nullptr);
    ~AddRegDialog();

signals:
    void readDefinitionsCreated(ModbusRegReadDefinitions *reg_def);
    void readDefinitionsModified(ModbusRegReadDefinitions *reg_def);

private slots:
    void on_button_ok_clicked();

    void on_box_id_valueChanged(int arg1);

    void on_box_function_currentTextChanged(const QString &arg1);

    void on_box_addr_valueChanged(int arg1);

    void on_box_quantity_valueChanged(int arg1);

    void on_button_cancel_clicked();

protected:
    void changeEvent(QEvent *event) override;

private:
    void generateRequest();

private:
    Ui::AddRegDialog *ui;
    ModbusRegReadDefinitions *m_reg_def;
    ModbusBase *m_modbus;
    QByteArray m_request_bytes;
    bool m_is_master;
};

#endif // ADDREGDIALOG_H
