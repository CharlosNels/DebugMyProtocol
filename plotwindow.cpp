#include "plotwindow.h"
#include "ui_plotwindow.h"
#include "qcustomplot.h"
#include "utils.h"
#include <QDateTime>
#include <QVBoxLayout>

PlotWindow::PlotWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlotWindow)
{
    ui->setupUi(this);
    m_layout = new QVBoxLayout();
    m_plot = new QCustomPlot(this);
    m_layout->addWidget(m_plot);
    m_x_time_ticker = QSharedPointer<QCPAxisTickerTime>(new QCPAxisTickerTime());
    m_x_time_ticker->setTimeFormat("%h:%m:%s");
    m_plot->xAxis->setTicker(m_x_time_ticker);
    m_plot->xAxis->setRangeLower(0);
    m_plot->xAxis->setRangeUpper(30);
}

void PlotWindow::addGraph(register_value_t reg_val)
{
    QTime time = QTime::currentTime();
    m_reg_list.append(reg_val);
    m_time_begin_map.insert(reg_val, time);
}

void PlotWindow::flushGraph()
{
    double value = 0;
    double x = 0;
    QTime now = QTime::currentTime();
    double max = 0;
    for(int i = 0;i < m_reg_list.size(); ++i)
    {
        value = get_value_by_format(&m_reg_list[i]);
        x = m_time_begin_map[m_reg_list[i]].secsTo(now);
        if(x > max)
        {
            max = x;
        }
        m_plot->graph(i)->addData(x, value);
    }
    if(max > m_plot->xAxis->range().upper * 0.8333)
    {
        m_plot->xAxis->setRangeUpper(max * 2);
    }
    m_plot->replot();
}


PlotWindow::~PlotWindow()
{
    delete ui;
}
