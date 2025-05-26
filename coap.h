#ifndef COAP_H
#define COAP_H

#include <QObject>

class CoAP : public QObject
{
    Q_OBJECT
    friend class ProtocolFactory;
private:
    explicit CoAP(QObject *parent = nullptr);
};

#endif // COAP_H
