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
class QCPItemText;

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();
    void addGraph(register_value_t reg_val);
    void flushGraph();
    bool isFull();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void plotMouseMoveEvent(QMouseEvent *event);

private:
    Ui::PlotWindow *ui;
    QVBoxLayout *m_layout;
    QCustomPlot *m_plot;
    QCPItemText *m_tip_item;
    QList<register_value_t> m_reg_list;
    QMap<register_value_t, QTime> m_time_begin_map;
    QSharedPointer<QCPAxisTickerTime> m_x_time_ticker;
    QList<QPen> m_graph_pens;
};

#endif // PLOTWINDOW_H
