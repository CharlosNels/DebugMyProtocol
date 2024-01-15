#ifndef DISPLAYCOMMUNICATION_H
#define DISPLAYCOMMUNICATION_H

#include <QDialog>

namespace Ui {
class DisplayCommunication;
}

class DisplayCommunication : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayCommunication(QWidget *parent = nullptr);
    ~DisplayCommunication();
    void appendPacket(const QString &packet, bool is_error);
protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void on_button_start_clicked();

    void on_button_clear_clicked();

    void on_button_save_clicked();

    void on_button_copy_clicked();

private:
    Ui::DisplayCommunication *ui;
    bool m_is_recording;
    QBrush m_origin_text_brush;
};

#endif // DISPLAYCOMMUNICATION_H
