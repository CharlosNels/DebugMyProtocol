#ifndef PROTOCOLWIDGET_H
#define PROTOCOLWIDGET_H

#include <QWidget>
#include <QIODevice>

class ProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProtocolWidget(QIODevice *com, int protocol, QWidget *parent = nullptr);

signals:

protected:
    void closeEvent(QCloseEvent *event)override;

protected:
    QIODevice *m_com;
    int m_protocol;
};

#endif // PROTOCOLWIDGET_H
