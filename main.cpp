#include "mainwindow.h"
#include "mapdefines.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <QDateTime>
#include <QMap>
#include <QDir>
#include <QTranslator>

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
    QFile file(QString("./Log/")+qAppName()+msg_type_map[msg_type]+".log");
    if(file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        QTextStream stream(&file);
        QString log = QString("%1 %2 %3 %4 : %5").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"),msg_ctx.file,msg_ctx.function).arg(msg_ctx.line).arg(msg);
        stream<<log;
        Qt::endl(stream);
        file.close();
    }
}

QStringList prog_dirs = {"./Config","./Log"};

void createProgDirs()
{
    for(auto &x : prog_dirs)
    {
        QDir dir;
        if(!dir.exists(x))
        {
            dir.mkdir(x);
        }
    }
}

int language_index = 0;
QTranslator translator;

int main(int argc, char *argv[])
{
    createProgDirs();
    qInstallMessageHandler(myMessageHandle);
    QApplication a(argc, argv);
    QFile trans_setting_file("./Config/translate_setting");
    if(trans_setting_file.open(QIODevice::ReadOnly))
    {
        QString trans_file_name = trans_setting_file.readAll();
        trans_setting_file.close();
        if(translator.load(trans_file_name))
        {
            if(trans_file_name.contains("zh_CN"))
            {
                language_index = 1;
            }
            QCoreApplication::installTranslator(&translator);
        }
    }
    MapDefine.init();
    MainWindow w;

    w.show();
    return a.exec();
}
