#include "plotdialog.h"
#include "ui_plotdialog.h"
#include "qcustomplot.h"
#include <QDateTime>

PlotDialog::PlotDialog(QList<register_value_t> reg_list, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlotDialog)
{
    ui->setupUi(this);
    m_reg_list = reg_list;
    m_plot = new QCustomPlot(this);
    ui->verticalLayout->addWidget(m_plot);
    m_x_time_ticker = QSharedPointer<QCPAxisTickerTime>(new QCPAxisTickerTime());
    m_x_time_ticker->setTimeFormat("%h:%m:%s");
    m_plot->xAxis->setTicker(m_x_time_ticker);
    m_plot->xAxis->setRangeLower(0);
    m_plot->xAxis->setRangeUpper(30);
    for(int i = 1; i < reg_list.size(); ++i)
    {
        m_plot->addGraph();

    }
}

PlotDialog::~PlotDialog()
{
    delete ui;
}
