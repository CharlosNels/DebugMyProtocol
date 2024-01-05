#ifndef FLOATBOX_H
#define FLOATBOX_H

#include <QWidget>

class FloatBox : public QObject
{
    Q_OBJECT
public:

    static void message(const QString &msg, quint32 duration,const QRect &rect);
signals:

private:
    explicit FloatBox(QWidget *parent = nullptr);
};

#endif // FLOATBOX_H
