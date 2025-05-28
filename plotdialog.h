#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>
#include <QList>
#include <QSharedPointer>

namespace Ui {
class PlotDialog;
}

struct register_value_t{
    int8_t *addr;
    int32_t format;
};

class QCustomPlot;
class QCPAxisTickerTime;

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QList<register_value_t> reg_list, QWidget *parent = nullptr);
    ~PlotDialog();

private:
    Ui::PlotDialog *ui;
    QCustomPlot *m_plot;
    QList<register_value_t> m_reg_list;
    QSharedPointer<QCPAxisTickerTime> m_x_time_ticker;
};

#endif // PLOTDIALOG_H
