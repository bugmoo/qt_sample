#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <QObject>
#include <QMutex>

class CLogMessage
{
public:
    CLogMessage();
    static void install(QString log_path);
    static void uninstall();

private:
    static QString m_logPath;
    static QMutex m_mutex;
    static void logMessageOut(QtMsgType type, const QMessageLogContext &context, const QString &msg);


};

#endif // LOGMESSAGE_H
