#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class OpenRouteDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:

    void on_actionOpen_Route_triggered();

    void routeCreated(QIODevice *com, QString name, int protocol, bool is_master);

    void on_actionLanguage_triggered();

private:
    Ui::MainWindow *ui;
    OpenRouteDialog *m_open_route_dialog;

};
#endif // MAINWINDOW_H
