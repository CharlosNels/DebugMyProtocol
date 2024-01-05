#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <QDateTime>
#include <QMap>

void myMessageHandle(QtMsgType msg_type, const QMessageLogContext& msg_ctx, const QString& msg)
{
    static const QMap<QtMsgType, QString> msg_type_map = {
                                                          {QtMsgType::QtDebugMsg, "Debug"},
        {QtMsgType::QtWarningMsg, "Warning"},
        {QtMsgType::QtCriticalMsg, "Critical"},
        {QtMsgType::QtFatalMsg, "Fatal"},
        {QtMsgType::QtInfoMsg, "Info"}
    };
    static QMutex mut; //多线程打印时需要加锁
    QMutexLocker locker(&mut);
    QFile file(qAppName()+msg_type_map[msg_type]+".log");
    if(file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        QTextStream stream(&file);
        QString log = QString("%1 %2 %3 %4 : %5").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"),msg_ctx.file,msg_ctx.function).arg(msg_ctx.line).arg(msg);
        stream<<log;
        Qt::endl(stream);
        file.close();
    }
}

int main(int argc, char *argv[])
{
    // qInstallMessageHandler(myMessageHandle);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
