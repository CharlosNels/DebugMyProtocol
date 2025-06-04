#include "testcenterwindow.h"
#include "ui_testcenterwindow.h"
#include "utils.h"
#include <QIODevice>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QClipboard>
#include <QFileDialog>
#include <QFile>

const QRegularExpression TestCenterWindow::Valid_Pack_Reg_Expr = QRegularExpression("^[0-9a-fA-F]{2}(?:([\x20,\\,,\\.])(?:(?:[0-9a-fA-F]{2}(?:\\1[0-9a-fA-F]{2})*\\1?)|$))?$");

TestCenterWindow::TestCenterWindow(QIODevice *com, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TestCenterWindow)
    , m_com(com)
{
    ui->setupUi(this);
}

TestCenterWindow::~TestCenterWindow()
{
    delete ui;
}

void TestCenterWindow::closeEvent(QCloseEvent *event)
{
    disconnect(m_com, &QIODevice::readyRead, this, &TestCenterWindow::comReadyReadSlot);
    ui->text_edit_traffic->clear();
    event->ignore();
    hide();
    emit closed();
}

void TestCenterWindow::showEvent(QShowEvent *event)
{
    if(isVisible())
    {
        return;
    }
    connect(m_com, &QIODevice::readyRead, this, &TestCenterWindow::comReadyReadSlot);
    m_packet_id = 0;
}

void TestCenterWindow::comReadyReadSlot()
{
    QByteArray recv_buffer = m_com->readAll();
    ui->text_edit_traffic->appendPlainText(QString("%1-Rx : %2\n").arg(m_packet_id++, 3, 10, QChar(u'0')).arg(QString(recv_buffer.toHex(' ').toUpper())));
}

void TestCenterWindow::on_button_send_clicked()
{
    QString pack_str = ui->box_test_list->currentText();
    if(pack_str.isEmpty() || !Valid_Pack_Reg_Expr.globalMatch(pack_str).hasNext())
    {
        return;
    }
    QChar seperator{};
    if(pack_str.contains(' '))
    {
        seperator = ' ';
    }
    else if(pack_str.contains(','))
    {
        seperator = ',';
    }
    else if(pack_str.contains('.'))
    {
        seperator = '.';
    }
    else
    {
        return;
    }
    QByteArray hex_pack = QByteArray::fromHex(pack_str.remove(seperator).toLatin1());
    if(ui->check_box_add_check->isChecked())
    {
        if(ui->radio_button_crc->isChecked())
        {
            quint16 crc_val = CRC_16(hex_pack, hex_pack.size());
            hex_pack.append(crc_val & 0xFF);
            hex_pack.append(crc_val >> 8 & 0xFF);
        }
        else
        {
            quint8 lrc_val = LRC(hex_pack, hex_pack.size());
            hex_pack.append(lrc_val);
        }
    }
    m_com->write(hex_pack);
    ui->text_edit_traffic->appendPlainText(QString("%1-Tx : %2\n").arg(m_packet_id++, 3, 10, QChar(u'0')).arg(QString(hex_pack.toHex(' ').toUpper())));
}


void TestCenterWindow::on_button_clear_clicked()
{
    ui->text_edit_traffic->clear();
}


void TestCenterWindow::on_button_copy_clicked()
{
    QApplication::clipboard()->setText(ui->text_edit_traffic->toPlainText());
}


void TestCenterWindow::on_button_add_to_list_clicked()
{
    if(ui->box_test_list->currentText().isEmpty() || m_test_list.contains(ui->box_test_list->currentText()))
    {
        return;
    }
    m_test_list << ui->box_test_list->currentText();
    ui->box_test_list->addItem(ui->box_test_list->currentText());
}


void TestCenterWindow::on_button_save_list_clicked()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save"), QString(), "Txt files(*.txt)");
    if(file_name.isEmpty())
    {
        return;
    }
    QFile test_list_file(file_name);
    if(test_list_file.open(QIODevice::WriteOnly))
    {
        for(auto &x : m_test_list)
        {
            test_list_file.write(QString(x + "\n").toUtf8());
        }
    }
}


void TestCenterWindow::on_button_open_list_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open"), QString(), "Txt files(*.txt)");
    if(file_name.isEmpty())
    {
        return;
    }
    QFile test_list_file(file_name);
    if(test_list_file.open(QIODevice::ReadOnly))
    {
        while(!test_list_file.atEnd())
        {
            QString line = test_list_file.readLine();
            line = line.remove("\r").remove("\n");
            if(!m_test_list.contains(line) && Valid_Pack_Reg_Expr.globalMatch(line).hasNext())
            {
                m_test_list << line;
                ui->box_test_list->addItem(line);
            }
        }
    }
}

