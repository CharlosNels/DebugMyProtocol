#ifndef MQTT_H
#define MQTT_H

#include <QObject>

class MQTT : public QObject
{
    Q_OBJECT
    friend class ProtocolFactory;
private:
    explicit MQTT(QObject *parent = nullptr);
};

#endif // MQTT_H
