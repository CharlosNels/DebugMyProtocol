#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSharedPointer>
#include <QTime>
#include "register_value_t.h"

namespace Ui {
class PlotWindow;
}

class QCustomPlot;
class QCPAxisTickerTime;
class QVBoxLayout;

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();
    void addGraph(register_value_t reg_val);
    void flushGraph();

private:
    Ui::PlotWindow *ui;
    QVBoxLayout *m_layout;
    QCustomPlot *m_plot;
    QList<register_value_t> m_reg_list;
    QMap<register_value_t, QTime> m_time_begin_map;
    QSharedPointer<QCPAxisTickerTime> m_x_time_ticker;
};

#endif // PLOTWINDOW_H
