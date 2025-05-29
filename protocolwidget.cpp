#include "protocolwidget.h"
#include <QIODevice>

ProtocolWidget::ProtocolWidget(QIODevice *com, int protocol, QWidget *parent)
    : QWidget{parent}, m_com{com}, m_protocol{protocol}
{

}

void ProtocolWidget::closeEvent(QCloseEvent *event)
{
    if(m_com)
    {
        m_com->deleteLater();
    }
}
