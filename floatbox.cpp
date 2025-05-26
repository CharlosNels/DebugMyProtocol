#include "floatbox.h"
#include <QFontMetrics>
#include <QApplication>
#include <QLabel>
#include <QTimer>

FloatBox::FloatBox(QWidget *parent)
    : QObject{parent}
{}

void FloatBox::message(const QString &msg, quint32 duration, const QRect &rect)
{
    int right_top_x = rect.x() + rect.width();
    int right_top_y = rect.y();
    QFontMetrics mec(QApplication::font());
    QLabel *label = new QLabel(msg,nullptr,Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setStyleSheet("border: 1px solid silver;");
    label->setMargin(30);
    int w = 0;
    int h = 0;
    int max_width = 400;
    QRect msg_rect = mec.boundingRect(msg);
    if(msg_rect.width() > max_width)
    {
        w = max_width;
        h = (mec.ascent() + mec.descent()) * ((msg_rect.width() / max_width) + 3);
    }
    else
    {
        w = msg_rect.width();
        h = (mec.ascent() + mec.descent()) * 4;
    }
    label->setGeometry(QRect(right_top_x - w - 30,right_top_y,w+30,h + 30));
    label->show();
    QTimer *t = new QTimer();
    connect(t, &QTimer::timeout, label, [label,t](){
        t->stop();
        t->deleteLater();
        label->deleteLater();
    });
    t->start(duration);
}
