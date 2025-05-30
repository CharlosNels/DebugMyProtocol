#include "plotwindow.h"
#include "ui_plotwindow.h"
#include "qcustomplot.h"
#include "utils.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QMessageBox>

PlotWindow::PlotWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlotWindow)
{
    ui->setupUi(this);
    m_layout = new QVBoxLayout(ui->centralwidget);
    m_plot = new QCustomPlot(this);
    m_plot->clearGraphs();
    m_layout->addWidget(m_plot);
    m_x_time_ticker = QSharedPointer<QCPAxisTickerTime>(new QCPAxisTickerTime());
    m_x_time_ticker->setTimeFormat("%h:%m:%s");
    m_plot->xAxis->setTicker(m_x_time_ticker);
    m_plot->xAxis->setRangeLower(0);
    m_plot->xAxis->setRangeUpper(30);
    m_plot->legend->setVisible(true);
    m_plot->setMouseTracking(true);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    connect(m_plot, &QCustomPlot::mouseMove, this, &PlotWindow::plotMouseMoveEvent);
    m_tip_item = new QCPItemText(m_plot);
    m_tip_item->setPen(QPen(Qt::black));
    m_tip_item->setVisible(false);
    m_graph_pens = {QColor::fromRgb(255, 0, 0),QColor::fromRgb(0, 255, 0),QColor::fromRgb(0, 0, 255),
                     QColor::fromRgb(255, 255, 0),QColor::fromRgb(0, 255, 255),QColor::fromRgb(255, 0, 255),
                    QColor::fromRgb(255, 165, 0),QColor::fromRgb(128, 0, 128),QColor::fromRgb(255, 105, 180),
                    QColor::fromRgb(0, 255, 127)};
}

void PlotWindow::addGraph(register_value_t reg_val)
{
    if(m_reg_list.size() >= m_graph_pens.size())
    {
        return;
    }
    QTime time = QTime::currentTime();
    m_reg_list.append(reg_val);
    m_time_begin_map.insert(reg_val, time);
    m_plot->addGraph();
    m_plot->graph(m_reg_list.size() - 1)->setPen(m_graph_pens[m_reg_list.size() - 1]);
    m_plot->graph(m_reg_list.size() - 1)->setName(reg_val.reg_name.isEmpty() ? QString::number(reg_val.reg_addr) : reg_val.reg_name);
    m_plot->graph(m_reg_list.size() - 1)->setSelectable(QCP::SelectionType::stSingleData);
}

void PlotWindow::flushGraph()
{
    if(m_reg_list.isEmpty())
    {
        return;
    }
    double value = 0;
    double x = 0;
    QTime now = QTime::currentTime();
    double x_max = 0;
    double y_max = m_plot->graph(0)->dataCount() > 0 ? m_plot->graph(0)->data()->at(0)->key : 0;
    for(int i = 0;i < m_reg_list.size(); ++i)
    {
        value = get_value_by_format(&m_reg_list[i]);
        if(value > y_max)
        {
            y_max = value;
        }
        x = m_time_begin_map[m_reg_list[i]].secsTo(now);
        if(x > x_max)
        {
            x_max = x;
        }
        m_plot->graph(i)->addData(x, value);
    }
    if(x_max > m_plot->xAxis->range().upper * 0.8333)
    {
        m_plot->xAxis->setRangeUpper(x_max * 2);
    }
    if(y_max > m_plot->yAxis->range().upper * 0.8333)
    {
        m_plot->yAxis->setRangeUpper(y_max * 2);
    }
    m_plot->replot();
}

bool PlotWindow::isFull()
{
    return m_reg_list.size() >= m_graph_pens.size();
}

void PlotWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    event->ignore();
    hide();
}

void PlotWindow::plotMouseMoveEvent(QMouseEvent *event)
{
    for(int i = 0; i < m_plot->graphCount(); ++i)
    {
        if(m_plot->graph(i)->visible())
        {
            QVariant details;
            double distance = m_plot->graph(i)->selectTest(event->pos(), false, &details);
            if(distance < 5)
            {
                QCPDataSelection data_selection = details.value<QCPDataSelection>();
                if(data_selection.dataPointCount() > 0)
                {
                    QTime begin_time(0,0,0); /*= m_time_begin_map[m_reg_list[i]];*/
                    auto it = m_plot->graph(i)->data()->at(data_selection.dataRange().begin());
                    QTime key_time = begin_time.addSecs(it->key);
                    m_tip_item->setText(QString("%1\ntime:%2\nvalue:%3").arg(m_plot->graph(i)->name()).arg(key_time.toString("hh:mm:ss")).arg(it->value));
                    QPointF tip_pos{(double)event->pos().x() + 35.0, event->pos().y() + 35.0};
                    m_tip_item->position->setPixelPosition(tip_pos);
                    m_tip_item->setVisible(true);
                    m_plot->replot();
                    return;
                }
            }
        }
    }
    m_tip_item->setVisible(false);
    m_plot->replot();
}


PlotWindow::~PlotWindow()
{
    delete ui;
}
