#include "displaycommunication.h"
#include "ui_displaycommunication.h"
#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QClipboard>


DisplayCommunication::DisplayCommunication(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DisplayCommunication)
{
    ui->setupUi(this);
    setModal(false);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    m_is_recording = true;
    m_origin_text_brush = ui->text_traffic->currentCharFormat().foreground();
    hide();
}

DisplayCommunication::~DisplayCommunication()
{
    delete ui;
}

void DisplayCommunication::appendPacket(const QString &packet, bool is_error)
{
    if(m_is_recording)
    {
        QTextCharFormat txt_fmt = ui->text_traffic->currentCharFormat();
        txt_fmt.setForeground(is_error ? Qt::red : m_origin_text_brush);
        ui->text_traffic->mergeCurrentCharFormat(txt_fmt);
        if(ui->box_timestamp->isChecked())
        {
            ui->text_traffic->appendPlainText(QString("%1 %2\n").arg(QDateTime::currentDateTime().toString("hh:mm:ss:zzz"),packet));
        }
        else
        {
            ui->text_traffic->appendPlainText(packet+"\n");
        }
        if(is_error && ui->box_stop_on_error->isChecked())
        {
            m_is_recording = false;
            ui->button_start->setText(tr("Start"));
        }
    }
}

void DisplayCommunication::closeEvent(QCloseEvent *event)
{
    m_is_recording = false;
    ui->button_start->setText(tr("Start"));
    ui->text_traffic->clear();
    hide();
    event->accept();
}

void DisplayCommunication::showEvent(QShowEvent *event)
{
    m_is_recording = true;
    ui->button_start->setText(tr("Stop"));
}

void DisplayCommunication::on_button_start_clicked()
{
    m_is_recording = !m_is_recording;
    ui->button_start->setText(m_is_recording ? tr("Stop") : tr("Start"));
}


void DisplayCommunication::on_button_clear_clicked()
{
    ui->text_traffic->clear();
}


void DisplayCommunication::on_button_save_clicked()
{
    QString file_path = QFileDialog::getSaveFileName(this, tr("Save Log"));
    if(file_path != "")
    {
        QFile traffic_file(file_path);
        if(traffic_file.open(QIODevice::WriteOnly))
        {
            traffic_file.write(ui->text_traffic->toPlainText().toUtf8());
            traffic_file.close();
        }
        else
        {
            qDebug()<<"Can not open : "<<file_path;
        }
    }
}


void DisplayCommunication::on_button_copy_clicked()
{
    QApplication::clipboard()->setText(ui->text_traffic->toPlainText());
}

