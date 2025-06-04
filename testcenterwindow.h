#ifndef TESTCENTERWINDOW_H
#define TESTCENTERWINDOW_H

#include <QMainWindow>
#include <QRegularExpression>

namespace Ui {
class TestCenterWindow;
}

class QIODevice;
//00 12 00 00 00 06 01 03 00 00 00 0A
class TestCenterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestCenterWindow(QIODevice *com, QWidget *parent = nullptr);
    ~TestCenterWindow();

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void comReadyReadSlot();

    void on_button_send_clicked();

    void on_button_clear_clicked();

    void on_button_copy_clicked();

    void on_button_add_to_list_clicked();

    void on_button_save_list_clicked();

    void on_button_open_list_clicked();

private:
    Ui::TestCenterWindow *ui;
    const static QRegularExpression Valid_Pack_Reg_Expr;
    QIODevice *m_com;
    quint32 m_packet_id;
    bool m_signal_connected;
    QList<QString> m_test_list;
};

#endif // TESTCENTERWINDOW_H
